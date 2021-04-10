/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import { Scene } from '../scene/Scene.js'
import { CStage, logger } from '../addon/index.js'
import { InputManager } from '../input/InputManager.js'
import { AudioManager } from '../audio/AudioManager.js'
import { logexcept, now, isPlainObject } from '../util/index.js'
import { PluginType } from '../addon/PluginType.js'
import { PluginId } from '../addon/PluginId.js'
import {
  createAttachedEvent,
  createDestroyedEvent,
  createDestroyingEvent,
  createDetachedEvent,
  createStartedEvent,
  createStoppedEvent
} from '../event/index.js'
import { EventName } from '../event/EventName.js'
import { readFileSync } from 'fs'
import { SystemManager } from '../system/SystemManager.js'
import { loadPlugin } from '../addon/loadPlugin.js'
import { EventTarget } from '../event/EventTarget.js'
import { FontManager } from '../font/FontManager.js'

const kFlagWasQuitRequested = 1
const kFlagIsAttached = 2
const kFlagIsConfigured = 4
const kFlagIsRunning = 8

/**
 * A stage object.
 *
 * @memberof module:@lse/core
 * @extends module:@lse/core.EventTarget
 * @hideconstructor
 */
class Stage extends EventTarget {
  _native = new CStage()
  _plugins = new Map()
  _scenes = new Map()
  _flags = 0
  _mainLoopHandle = null
  _frameRate = 0
  _loadPlugin = null
  $scene = null

  constructor (loadPluginFunc = loadPlugin) {
    super([
      EventName.attached,
      EventName.detached,
      EventName.started,
      EventName.stopped,
      EventName.destroying,
      EventName.destroyed
    ])

    // loadPlugin injectable for testability
    this._loadPlugin = loadPluginFunc

    Object.defineProperties(this, {
      font: { value: new FontManager() },
      input: { value: new InputManager(this) },
      audio: { value: new AudioManager() },
      system: { value: new SystemManager() }
    })
  }

  configure (appConfig = undefined) {
    if (this.isConfigured()) {
      throw Error('stage can only be configured once')
    }

    if (!appConfig || typeof appConfig === 'string') {
      appConfig = loadAppConfigFromFile(appConfig)
    }

    const {
      plugin = [PluginId.SDL, [PluginId.SDL_MIXER, PluginId.SDL_AUDIO, PluginId.NULL]],
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

    this.dispatchEvent(createStartedEvent(this))
  }

  stop () {
    if (!this.isRunning()) {
      return
    }

    // TODO: ...
    this.$detach()
    this.$stopMainLoop()
    this._flags &= ~kFlagIsRunning
    this.dispatchEvent(createStoppedEvent(this))
  }

  createScene (sceneConfig = {}) {
    if (!this.isConfigured()) {
      throw Error('stage must be configured before calling createScene')
    }

    if (this._scenes.size > 0) {
      throw Error('stage can only manage 1 scene at a time.')
    }

    const scene = new Scene(this, sceneConfig)

    scene.once(EventName.destroying, ({ target }) => {
      this._scenes.delete(target.displayIndex)
      if (target === this.$scene) {
        this.$scene = null
      }
    })

    this._scenes.set(scene.displayIndex, scene)

    if (!this.$scene) {
      this.$scene = scene
    }

    if (this.isAttached()) {
      try {
        scene.$attach()
      } catch (e) {
        logexcept(() => scene.destroy(), 'createScene()')
        throw e
      }

      this.$updateFrameRate()
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

  getScene (displayId) {
    return (displayId === -1) ? this.$scene : this._scenes.get(displayId)
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
    if (!this._native) {
      return
    }

    this.$detach()

    const site = 'stage.$destroy()'

    this.dispatchEvent(createDestroyingEvent(this))

    this.$stopMainLoop()

    this._scenes.forEach(scene => logexcept(() => scene.$destroy(), site))
    this._scenes.clear()

    listManagers(this).reverse().forEach(manager => logexcept(() => manager.$destroy(), site))

    listPlugins(this).reverse().forEach(plugin => logexcept(() => plugin?.destroy(), site))
    this._plugins.clear()

    this._flags = 0
    this._native.destroy()
    this._native = null

    this.dispatchEvent(createDestroyedEvent(this))

    // TODO: clear event target listeners?
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
    this.$updateFrameRate()

    this._flags |= kFlagIsAttached
    this.dispatchEvent(createAttachedEvent(this))
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
    this.dispatchEvent(createDetachedEvent(this))
  }

  /**
   * @ignore
   */
  $stopMainLoop () {
    clearTimeout(this._mainLoopHandle)
    this._mainLoopHandle = null
  }

  $updateFrameRate () {
    this._frameRate = this.$scene?._context.getRefreshRate() || 60
  }

  /**
   * @ignore
   */
  $setupPlugins (config) {
    const site = '$setupPlugins'
    const ensurePluginConfigObject = (obj) => {
      if (isPlainObject(obj) && 'id' in obj) {
        return obj
      }
      throw Error(`Invalid plugin config entry: [${obj?.toString()}]`)
    }

    // Convert all string plugin entries to a name/options Object.
    config = config.map(configEntry => {
      if (typeof configEntry === 'string') {
        return { id: configEntry }
      } else if (Array.isArray(configEntry)) {
        return configEntry.map(entry => {
          if (typeof entry === 'string') {
            return { id: entry }
          } else {
            return ensurePluginConfigObject(entry)
          }
        })
      } else {
        return ensurePluginConfigObject(configEntry)
      }
    })

    const installPlugin = (pluginConfig) => {
      if (pluginConfig.id === PluginId.NULL) {
        return
      }

      const plugin = this._loadPlugin(pluginConfig.id, pluginConfig.options)
      const { type } = plugin

      if (this._plugins.has(type)) {
        throw Error(`plugin of type ${type} has already been loaded.`)
      }

      plugin.attach()

      this._plugins.set(type, plugin)

      // TODO: review lifecycle management of plugins.
      switch (type) {
        case PluginType.PLATFORM:
          try {
            this.system.$setPlugin(plugin)
            this.system.$attach()
            this.input.$setPlugin(plugin)
            this.input.$attach()
          } catch (e) {
            logexcept(() => this.system.$detach(), site)
            logexcept(() => this.system.$setPlugin(null), site)
            logexcept(() => this.input.$detach(), site)
            logexcept(() => this.input.$setPlugin(null), site)
            logexcept(() => plugin.detach(), site)
            this._plugins.delete(type)
            throw e
          }
          break
        case PluginType.AUDIO:
          try {
            // TODO: plugin attached inside audio manager (need to move here)
            this.audio.$setPlugin(plugin)
            this.audio.$attach()
          } catch (e) {
            logexcept(() => this.audio.$detach(), site)
            logexcept(() => this.audio.$setPlugin(null), site)
            logexcept(() => plugin.detach(), site)
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

  /**
   * @ignore
   */
  get $native () {
    return this._native
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

const listManagers = (stage) => [stage.font, stage.system, stage.input, stage.audio]

export { Stage }
