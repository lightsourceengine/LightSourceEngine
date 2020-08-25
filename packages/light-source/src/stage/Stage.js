/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from '../scene/Scene'
import bindings from 'bindings'
import { StageBase, SDLModuleId, SDLMixerModuleId, SDLAudioModuleId } from '../addon'
import { performance } from 'perf_hooks'
import { InputManager } from '../input/InputManager'
import { EventEmitter } from '../util/EventEmitter'
import { AudioManager } from '../audio/AudioManager'
import { isNumber, logexcept } from '../util'

const { now } = performance

const kEmptyPlatformPlugin = { capabilities: { displays: [] } }
const kPluginTypeAudio = 'audio'
const kPluginTypePlatform = 'platform'
const kPluginTypes = new Set([kPluginTypeAudio, kPluginTypePlatform])

export class Stage extends StageBase {
  _mainLoopHandle = null
  _fps = 60
  _scene = null
  _platformPlugin = null
  _audioPlugin = null
  _inputManager = new InputManager(this)
  _audioManager = new AudioManager(this)
  _emitter = new EventEmitter()
  _quitRequested = false

  constructor () {
    super()

    process.on('exit', () => {
      logexcept(() => this.$destroy(), 'exit: Stage destroy exception: ')
      global.gc && global.gc()
    })
  }

  get capabilities () {
    return (this._platformPlugin || kEmptyPlatformPlugin).capabilities
  }

  get fps () {
    return this._fps
  }

  set fps (value) {
    if (!isNumber(value) || value < 0 || value > 60) {
      throw Error()
    }

    this._fps = value || 60
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

  /**
   *
   */
  init () {
    // TODO: need init check? if so, better check?
    if (this._platformPlugin || this._audioPlugin) {
      throw Error('Stage has already been initialized.')
    }

    try {
      this.loadPlugin(SDLModuleId, {})
    } catch (e) {
      throw Error('Failed to load light-source-sdl.node')
    }

    try {
      this.loadPlugin(SDLMixerModuleId, {})
    } catch (e) {
      try {
        this.loadPlugin(SDLAudioModuleId, {})
      } catch (e) {
        throw Error('Failed to load audio plugin.')
      }
    }
  }

  /**
   *
   * @param plugin
   * @param config
   */
  loadPlugin (plugin, config = {}) {
    let Plugin

    if (typeof plugin === 'string') {
      try {
        Plugin = bindings(plugin)
      } catch (e) {
        throw Error(`Failed to load Plugin '${plugin}'. Error: ${e.message}`)
      }
    }

    const { type } = validatePluginAddonObject(Plugin)

    if (this.hasPlugin(type)) {
      throw Error(`Plugin '${type}' has already loaded.`)
    }

    let instance

    try {
      instance = Plugin.createInstance()
    } catch (e) {
      throw Error(`Failed to create Plugin instance. Error: ${e.message}`)
    }

    switch (type) {
      case kPluginTypePlatform:
        try {
          instance.attach()
        } catch (e) {
          throw Error(`Failed to attach Plugin instance. Error: ${e.message}`)
        }
        setupPluginInstance(this.input, instance, (instance) => {
          this._platformPlugin = instance
          this.input.$setPlugin(instance)
        })
        break
      case kPluginTypeAudio:
        setupPluginInstance(this.audio, instance,
          (instance) => {
            this._audioPlugin = instance
            this.audio.$setPlugin(instance)
          })
        break
    }
  }

  hasPlugin (pluginType) {
    switch (pluginType) {
      case kPluginTypeAudio:
        return !!this._audioPlugin
      case kPluginTypePlatform:
        return !!this._platformPlugin
      default:
        return false
    }
  }

  on (id, listener) {
    this._emitter.on(id, listener)
  }

  off (id, listener) {
    this._emitter.off(id, listener)
  }

  once (id, listener) {
    this._emitter.once(id, listener)
  }

  createScene (config = {}) {
    if (!this.hasPlugin(kPluginTypePlatform)) {
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

    return scene
  }

  getScene (displayIndex = 0) {
    // TODO: validate displayIndex?
    return this._scene
  }

  start () {
    // TODO: initialized?
    // TODO: check already started
    if (this._mainLoopHandle) {
      return
    }

    const platform = this._platformPlugin
    const scene = this._scene
    let lastTick = now()

    this._attach()

    const mainLoop = () => {
      const tick = now()

      if (!platform.processEvents() || this._quitRequested) {
        // TODO: revisit stage lifecycle...
        process.exit()
      }

      // TODO: check suspended

      scene.$frame(tick, lastTick)
      lastTick = tick

      this._mainLoopHandle = setTimeout(mainLoop, 1000 / this._fps)
    }

    this._mainLoopHandle = setTimeout(mainLoop, 0)
  }

  stop () {
    // TODO: stopped?
    if (this._mainLoopHandle) {
      // TODO: handle exceptions...
      this._detach()

      clearTimeout(this._mainLoopHandle)
      this._mainLoopHandle = null
    }
  }

  // TODO: running flag? or state?

  quit () {
    this._quitRequested = true
  }

  /**
   * @ignore
   */
  $emit (event) {
    this._emitter.emit(event)
  }

  /**
   * @ignore
   */
  $destroy () {
    super.$destroy()

    this._scene && this._scene.$destroy()
    this.audio && this.audio.$destroy()
    this.input && this.input.$destroy()
    this._audioPlugin && this._audioPlugin.destroy()
    this._platformPlugin && this._platformPlugin.destroy()
  }

  /**
   * @ignore
   */
  _attach () {
    this._platformPlugin.attach()

    this.input.$attach()
    this.audio.$attach()
    this._scene.$attach()
  }

  /**
   * @ignore
   */
  _detach () {
    this._scene && this._scene.$detach()
    this.audio && this.audio.$detach()
    this.input && this.input.$detach()

    this._platformPlugin && this._platformPlugin.detach()
  }
}

const setupPluginInstance = (manager, pluginInstance, setPluginInstance) => {
  setPluginInstance(pluginInstance)

  try {
    manager.$attach()
  } catch (e) {
    setPluginInstance(null)
    throw Error(`Failed to attach Plugin instance. Error: ${e.message}`)
  }
}

const validatePluginAddonObject = (object) => {
  if (typeof object !== 'object' || typeof object.createInstance !== 'function' ||
      !kPluginTypes.has(object.type)) {
    throw Error('Invalid plugin addon objet.')
  }

  return object
}
