/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from '../scene/Scene.js'
import { addonError, loadPluginById, logger, PluginId, StageBase } from '../addon/index.js'
import { InputManager } from '../input/InputManager.js'
import { AudioManager } from '../audio/AudioManager.js'
import { isNumber, logexcept, EventEmitter, now, isPlainObject } from '../util/index.js'
import { PluginType } from '../addon/PluginType.js'
import {
  AttachedEvent,
  DestroyedEvent,
  DestroyingEvent,
  DetachedEvent,
  StartedEvent,
  StoppedEvent,
  EventNames
} from '../event/index.js'
import { readFileSync } from 'fs'
import { SystemManager } from '../system/SystemManager.js'

const kFlagWasQuitRequested = 1
const kFlagIsAttached = 2
const kFlagIsConfigured = 4
const kFlagIsRunning = 8

export class Stage extends StageBase {
  _emitter = new EventEmitter([
    EventNames.attached,
    EventNames.detached,
    EventNames.started,
    EventNames.stopped,
    EventNames.destroying,
    EventNames.destroyed
  ])

  _plugins = new Map()
  _scenes = new Map()
  _flags = 0
  _mainLoopHandle = null
  _frameRate = 0

  constructor (loadPluginByIdFunc = loadPluginById) {
    super()
    // injected for testability
    this._loadPluginById = loadPluginByIdFunc
    Object.defineProperties(this, {
      input: { value: new InputManager(this) },
      audio: { value: new AudioManager(this) },
      system: { value: new SystemManager() }
    })
    this.resetFrameRate()
  }

  on (id, listener) {
    this._emitter.on(id, listener)
  }

  once (id, listener) {
    this._emitter.once(id, listener)
  }

  off (id, listener) {
    this._emitter.off(id, listener)
  }

  configure (appConfig = undefined) {
    if (addonError) {
      throw Error(`Failed to load lse-core.node: ${addonError.message}`)
    }

    if (this.isConfigured()) {
      throw Error('stage can only be configured once')
    }

    if (!appConfig || typeof appConfig === 'string') {
      appConfig = loadAppConfigFromFile(appConfig)
    }

    const {
      plugin = [PluginId.SDL, [PluginId.SDL_MIXER, PluginId.SDL_ADIO, PluginId.NULL]],
      scene,
      stage
    } = appConfig

    this.$setupPlugins(plugin)

    this._flags |= kFlagIsConfigured

    let createdScene

    if (scene) {
      createdScene = this.createScene(scene)
    }

    if (stage?.start ?? true) {
      this.start()
    }

    return { scene: createdScene }
  }

  start () {
    if (this.isRunning()) {
      return
    }

    if (!this.isConfigured()) {
      throw Error('stage must be configured to start app')
    }

    if (!this.isAttached()) {
      this.$attach()
    }

    let lastTick = now()

    const mainLoop = () => {
      const tick = now()
      const sleepTime = (1000 / this._frameRate) - (tick - lastTick)

      if (!this.system.$processEvents() || this.wasQuitRequested()) {
        this.$destroy()
        return
      }

      if (this.isRunning()) {
        this._scenes.forEach(scene => scene.$frame(tick, lastTick))
      }

      lastTick = tick

      if (this.isRunning()) {
        this._mainLoopHandle = setTimeout(mainLoop, sleepTime < 0 ? 0 : sleepTime)
      }
    }

    this._flags |= kFlagIsRunning
    queueMicrotask(mainLoop)
    this._emitter.emitEvent(StartedEvent(this))
  }

  stop () {
    if (!this.isRunning()) {
      return
    }

    // TODO: ...
    this.$detach()
    this.$stopMainLoop()
    this._flags &= ~kFlagIsRunning
    this._emitter.emitEvent(StoppedEvent(this))
  }

  createScene (sceneConfig = {}) {
    if (!this.isConfigured()) {
      throw Error('stage must be configured before calling createScene')
    }

    if (this._scenes.size > 0) {
      throw Error('stage can only manage 1 scene at a time.')
    }

    const scene = new Scene(this, sceneConfig)

    scene.once(EventNames.destroying, ({ target }) => this._scenes.delete(target.displayIndex))

    this._scenes.set(scene.displayIndex, scene)

    if (this.isAttached()) {
      try {
        scene.$attach()
      } catch (e) {
        logexcept(() => scene.destroy(), 'createScene()')
        throw e
      }
    }

    return scene
  }

  quit () {
    if (this.wasQuitRequested()) {
      return
    }

    this._flags |= kFlagWasQuitRequested

    if (!this.isRunning()) {
      queueMicrotask(() => this.$destroy())
    }
  }

  getFrameRate () {
    return this._frameRate
  }

  setFrameRate (value) {
    if (!isNumber(value) || value <= 0) {
      throw Error('frameRate must be a number greater than 0')
    }

    this._frameRate = value
  }

  resetFrameRate () {
    this._frameRate = 60
  }

  getScene (displayId) {
    if (displayId === -1) {
      displayId = this.system.displays.findIndex(display => this._scenes.has(display.id))
    }

    return this._scenes.get(displayId)
  }

  isRunning () {
    return !!(this._flags & kFlagIsRunning)
  }

  isAttached () {
    return !!(this._flags & kFlagIsAttached)
  }

  isConfigured () {
    return !!(this._flags & kFlagIsConfigured)
  }

  wasQuitRequested () {
    return !!(this._flags & kFlagWasQuitRequested)
  }

  hasPluginType (type) {
    return this._plugins.has(type)
  }

  /**
   * @ignore
   */
  $destroy () {
    const site = 'stage.$destroy()'

    this._emitter.emitEvent(DestroyingEvent(this))

    super.$destroy()

    this._scenes.forEach(scene => logexcept(() => scene.$destroy(), site))
    this._scenes.clear()

    listManagers(this).reverse().forEach(manager => logexcept(() => manager.$destroy(), site))

    listPlugins(this).reverse().forEach(plugin => logexcept(() => plugin?.destroy(), site))
    this._plugins.clear()

    this.$stopMainLoop()
    this._flags = 0

    this._emitter.emitEvent(DestroyedEvent(this))
  }

  /**
   * @ignore
   */
  $attach () {
    if (this.isAttached()) {
      return
    }

    listPlugins(this).forEach(plugin => plugin?.attach())

    listManagers(this).forEach(manager => manager.$attach())
    this._scenes.forEach(scene => scene.$attach())

    this._flags |= kFlagIsAttached
    this._emitter.emitEvent(AttachedEvent(this))
  }

  /**
   * @ignore
   */
  $detach () {
    if (!this.isAttached()) {
      return
    }

    this._scenes.forEach(scene => scene.$detach())
    listManagers(this).reverse().forEach(manager => manager.$detach())
    listPlugins(this).reverse().forEach(plugin => plugin?.detach())

    this._flags &= ~kFlagIsAttached
    this._emitter.emitEvent(DetachedEvent(this))
  }

  /**
   * @ignore
   */
  $stopMainLoop () {
    clearTimeout(this._mainLoopHandle)
    this._mainLoopHandle = null
  }

  /**
   * @ignore
   */
  $setupPlugins (config) {
    const site = '$setupPlugins'

    // Convert all string plugin entries to a name/options Object.
    config = config.map(value => {
      if (typeof value === 'string') {
        return { id: value }
      } else if (Array.isArray(value)) {
        return value.map(subValue => typeof value === 'string' ? { name: subValue } : value)
      } else {
        return value
      }
    })

    const installPlugin = (pluginConfig) => {
      if (pluginConfig.id === PluginId.NULL) {
        return
      }

      const plugin = this._loadPluginById(pluginConfig.id)
      const { type } = plugin

      if (this._plugins.has(type)) {
        throw Error(`plugin of type ${type} has already been loaded.`)
      }

      let instance

      // TODO: check is here while plugin api is being refactored
      if (plugin.createInstance) {
        instance = plugin.createInstance()
      } else {
        instance = plugin
      }

      this._plugins.set(type, instance)

      // TODO: review lifecycle management of plugins.
      switch (type) {
        case PluginType.PLATFORM:
          try {
            instance?.attach()
            this.system.$setPlugin(instance)
            this.system.$attach()
            this.input.$setPlugin(instance)
            this.input.$attach()
          } catch (e) {
            logexcept(() => this.system.$detach(), site)
            logexcept(() => this.system.$setPlugin(null), site)
            logexcept(() => this.input.$detach(), site)
            logexcept(() => this.input.$setPlugin(null), site)
            logexcept(() => instance?.detach(), site)
            this._plugins.delete(type)
            throw e
          }
          break
        case PluginType.AUDIO:
          try {
            // TODO: plugin attached inside audio manager (need to move here)
            this.audio.$setPlugin(instance)
            this.audio.$attach()
          } catch (e) {
            logexcept(() => this.audio.$detach(), site)
            logexcept(() => this.audio.$setPlugin(null), site)
            logexcept(() => instance?.detach(), site)
            this._plugins.delete(type)
            throw e
          }
          break
        default:
          this._plugins.delete(type)
          throw Error(`Unsupported plugin type: ${type}`)
      }
    }

    for (const pluginConfig of config) {
      if (Array.isArray(pluginConfig)) {
        for (const plugin of pluginConfig) {
          try {
            installPlugin(plugin)
            logger.info(`Loaded plugin ${plugin.id}`)
          } catch (e) {
            logger.warn(`Failed to load plugin ${plugin.id}: ${e.message}`, site)
            continue
          }

          break
        }
      } else {
        installPlugin(pluginConfig)
        logger.info(`Loaded plugin ${pluginConfig.id}`)
      }
    }
  }
}

const loadAppConfigFromFile = (filename) => {
  let fileMustExist
  let contents
  let appConfig

  if (typeof filename === 'string') {
    fileMustExist = true
  } else {
    filename = process.env.LSE_APP_CONFIG ?? 'config.json'
    fileMustExist = false
  }

  try {
    contents = readFileSync(filename, 'utf8')
  } catch (e) {
    if (fileMustExist) {
      throw Error(`app config JSON file not found: ${filename}`)
    } else {
      return {}
    }
  }

  try {
    appConfig = JSON.parse(contents)
  } catch (e) {
    throw Error(`app config: '${filename}'. Error: ${e.message}`)
  }

  if (!isPlainObject(appConfig)) {
    throw Error(`app config: '${filename}'. Error: Expected app config file to be a JSON object.`)
  }

  return appConfig
}

const listPlugins = ({ _plugins }) => [_plugins.get(PluginType.PLATFORM), _plugins.get(PluginType.AUDIO)]

const listManagers = (stage) => [stage.system, stage.input, stage.audio]
