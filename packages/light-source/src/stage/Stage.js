/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from '../scene/Scene.js'
import { addonError, loadPlugin, logger, StageBase } from '../addon/index.js'
import { InputManager } from '../input/InputManager.js'
import { AudioManager } from '../audio/AudioManager.js'
import { isNumber, logexcept, EventEmitter, now } from '../util/index.js'
import { PluginType } from './PluginType.js'
import {
  AttachedEvent,
  DestroyedEvent,
  DestroyingEvent,
  DetachedEvent,
  EventNames,
  StartedEvent,
  StoppedEvent
} from '../event/index.js'

const kEmptyCapabilities = { displays: [] }

export class Stage extends StageBase {
  _mainLoopHandle = null
  _frameRate = 60
  _scene = null
  _platformPlugin = null
  _audioPlugin = null
  _inputManager = new InputManager(this)
  _audioManager = new AudioManager(this)
  _emitter = new EventEmitter([
    EventNames.attached,
    EventNames.detached,
    EventNames.started,
    EventNames.stopped,
    EventNames.destroying,
    EventNames.destroyed
  ])

  _quitRequested = false
  _attached = false
  _running = false

  on (id, listener) {
    this._emitter.on(id, listener)
  }

  once (id, listener) {
    this._emitter.once(id, listener)
  }

  off (id, listener) {
    this._emitter.off(id, listener)
  }

  get capabilities () {
    return this._platformPlugin?.capabilities ?? kEmptyCapabilities
  }

  get input () {
    return this._inputManager
  }

  /**
   * Access the audio API for playing sound effects and background music.
   *
   * The native audio support that backs the AudioManager can be initialized through Stage.init(), using the
   * audioAdapter option.
   *
   * @returns {AudioManager}
   */
  get audio () {
    return this._audioManager
  }

  getFrameRate () {
    return this._frameRate
  }

  setFrameRate (value) {
    if (!isNumber(value) || value < 0 || value > 60) {
      throw Error('frameRate must be a number between [0, 60]')
    }

    this._frameRate = value || 60
  }

  /**
   *
   */
  init () {
    if (this._platformPlugin) {
      throw Error('Stage platform plugin has already been initialized.')
    }

    this.loadPlugin('platform:sdl')

    if (!this._audioPlugin) {
      const site = 'init()'
      const mixer = 'audio:sdl-mixer'
      const audio = 'audio:sdl-audio'

      try {
        this.loadPlugin(mixer)
      } catch (mixerError) {
        try {
          this.loadPlugin(audio)
        } catch (audioError) {
          logger.warn(`${mixer}: ${mixerError.message}`, site)
          logger.warn(`${audio}: ${audioError.message}`, site)
        }
      }
    }
  }

  /**
   *
   * @param plugin
   * @param config
   */
  loadPlugin (plugin, config = {}) {
    if (addonError) {
      throw Error(`Failed to load light-source.node: ${addonError.message}`)
    }

    const site = 'loadPlugin()'
    const Plugin = loadPlugin(plugin)

    assertPluginInterface(Plugin)

    const { type } = Plugin

    if (this.hasPlugin(type)) {
      throw Error(`Plugin '${type}' has already loaded.`)
    }

    const instance = Plugin.createInstance()

    switch (type) {
      case PluginType.Platform:
        try {
          instance.attach()
        } catch (e) {
          logexcept(() => instance.destroy(), site)
          throw e
        }

        this._inputManager.$setPlugin(instance)

        try {
          this._inputManager.$attach()
        } catch (e) {
          logexcept(instance.destroy(), site)
          this._inputManager.$setPlugin(null)
          throw e
        }

        this._platformPlugin = instance
        logger.info(`${type} plugin loaded from ${typeof plugin === 'string' ? plugin : 'object'}`, site)
        break
      case PluginType.Audio:
        this._audioManager.$setPlugin(instance)

        try {
          this._audioManager.$attach()
        } catch (e) {
          logexcept(() => instance.destroy(), site)
          this._audioManager.$setPlugin(null)
          throw e
        }

        this._audioPlugin = instance
        logger.info(`${type} plugin loaded from ${typeof plugin === 'string' ? plugin : 'object'}`, site)
        break
    }
  }

  hasPlugin (pluginType) {
    switch (pluginType) {
      case PluginType.Audio:
        return !!this._audioPlugin
      case PluginType.Platform:
        return !!this._platformPlugin
      default:
        return false
    }
  }

  createScene (config = {}) {
    if (!this._platformPlugin) {
      this.init()
    }

    if (this._scene) {
      throw Error('Stage can only manage 1 scene at a time.')
    }

    let scene

    try {
      scene = new Scene(this, this._platformPlugin, config)
    } catch (e) {
      throw Error(e.message)
    }

    this._scene = scene

    if (this.isAttached()) {
      scene.$attach()
    }

    return scene
  }

  get scene () {
    return this._scene
  }

  getScene (displayIndex = 0) {
    // TODO: validate displayIndex?
    return this._scene
  }

  start () {
    if (this._running) {
      return
    }

    if (!this.isAvailable()) {
      throw Error()
    }

    let lastTick = now()

    this._attach()

    const mainLoop = () => {
      const tick = now()

      if (!this._platformPlugin.processEvents() || this._quitRequested) {
        logexcept(() => this.$destroy(), 'mainLoopExit')
        return
      }

      // TODO: check suspended

      this._scene?.$frame(tick, lastTick)
      lastTick = tick

      if (this._running) {
        this._mainLoopHandle = setTimeout(mainLoop, 1000 / this._frameRate)
      }
    }

    this._running = true
    this._mainLoopHandle = setTimeout(mainLoop, 0)
    this._emitter.emitEvent(StartedEvent(this))
  }

  stop () {
    if (!this._running) {
      return
    }

    this._detach()
    clearTimeout(this._mainLoopHandle)
    this._mainLoopHandle = null

    this._emitter.emitEvent(StoppedEvent(this))
  }

  quit () {
    if (this._running) {
      this._quitRequested = true
    } else {
      queueMicrotask(() => {
        logexcept(() => this.$destroy(), 'quit()')
      })
    }
  }

  isRunning () {
    return this._running
  }

  isAttached () {
    return this._attached
  }

  isAvailable () {
    return !!this._platformPlugin
  }

  /**
   * @ignore
   */
  $destroy () {
    this._emitter.emitEvent(DestroyingEvent(this))

    super.$destroy()

    this._audioManager?.$destroy()
    this._inputManager?.$destroy()
    this._scene?.$destroy()
    this._scene = null
    this._platformPlugin?.destroy()
    this._platformPlugin = null

    this._emitter.emitEvent(DestroyedEvent(this))
  }

  /**
   * @ignore
   */
  _attach () {
    if (this._attached) {
      return
    }

    this._platformPlugin.attach()

    this._inputManager.$attach()
    this._audioManager.$attach()
    this._scene?.$attach()

    this._attached = true
    this._emitter.emitEvent(AttachedEvent(this))
  }

  /**
   * @ignore
   */
  _detach () {
    if (!this._attached) {
      return
    }

    this._scene?.$detach()
    this._audioManager?.$detach()
    this._inputManager?.$detach()

    this._platformPlugin?.detach()

    this._attached = false
    this._emitter.emitEvent(DetachedEvent(this))
  }
}

const assertPluginInterface = (object) => {
  if (typeof object !== 'object' || typeof object.createInstance !== 'function' ||
      !Object.values(PluginType).includes(object.type)) {
    throw Error('Expected a plugin object with createInstance() and type properties.')
  }
}
