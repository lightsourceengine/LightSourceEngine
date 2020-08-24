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
import {
  $mainLoopHandle,
  $fps,
  $attach,
  $detach,
  $destroy,
  $frame,
  $quitRequested,
  $input,
  $events,
  $scene,
  $audio,
  $emit,
  $init
} from '../util/InternalSymbols'
import { AudioManager } from '../audio/AudioManager'
import { isNumber, logexcept } from '../util'

const { now } = performance

const $platformPlugin = Symbol.for('platformPlugin')
const $audioPlugin = Symbol.for('audioPlugin')

const kEmptyPlatformPlugin = { capabilities: { displays: [] } }
const kPluginTypeAudio = 'audio'
const kPluginTypePlatform = 'platform'
const kPluginTypes = new Set([kPluginTypeAudio, kPluginTypePlatform])

export class Stage extends StageBase {
  constructor () {
    super()

    this[$mainLoopHandle] = null
    this[$fps] = 60
    this[$scene] = null
    this[$platformPlugin] = null
    this[$audioPlugin] = null
    this[$input] = new InputManager(this)
    this[$audio] = new AudioManager(this)
    this[$events] = new EventEmitter()

    process.on('exit', () => {
      logexcept(() => this[$destroy](), 'exit: Stage destroy exception: ')
      global.gc && global.gc()
    })
  }

  get capabilities () {
    return (this[$platformPlugin] || kEmptyPlatformPlugin).capabilities
  }

  get fps () {
    return this[$fps]
  }

  set fps (value) {
    if (!isNumber(value) || value < 0 || value > 60) {
      throw Error()
    }

    this[$fps] = value || 60
  }

  get input () {
    return this[$input]
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
    return this[$audio]
  }

  /**
   *
   */
  init () {
    // TODO: need init check? if so, better check?
    if (this[$platformPlugin] || this[$audioPlugin]) {
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
        this[$platformPlugin] = instance
        // this[$input][$init](stageAdapter, gameControllerDb)
        break
      case kPluginTypeAudio:
        this[$audioPlugin] = instance
        this[$audio][$init](instance)
        break
    }
  }

  hasPlugin (pluginType) {
    switch (pluginType) {
      case kPluginTypeAudio:
        return !!this[$audioPlugin]
      case kPluginTypePlatform:
        return !!this[$platformPlugin]
      default:
        return false
    }
  }

  on (id, listener) {
    this[$events].on(id, listener)
  }

  off (id, listener) {
    this[$events].off(id, listener)
  }

  once (id, listener) {
    this[$events].once(id, listener)
  }

  createScene (config = {}) {
    if (!this.hasPlugin(kPluginTypePlatform)) {
      this.init()
    }

    if (this[$scene]) {
      throw Error('Stage can only manage 1 scene at a time.')
    }

    let scene

    try {
      scene = new Scene(this, this[$platformPlugin], config)
    } catch (e) {
      throw Error(e.message)
    }

    this[$scene] = scene

    return scene
  }

  getScene (displayIndex = 0) {
    // TODO: validate displayIndex?
    return this[$scene]
  }

  start () {
    // TODO: initialized?
    // TODO: check already started
    if (this[$mainLoopHandle]) {
      return
    }

    const platform = this[$platformPlugin]
    const scene = this[$scene]
    let lastTick = now()

    this[$attach]()

    const mainLoop = () => {
      const tick = now()

      if (!platform.processEvents() || this[$quitRequested]) {
        // TODO: revisit stage lifecycle...
        process.exit()
      }

      // TODO: check suspended

      scene[$frame](tick, lastTick)
      lastTick = tick

      this[$mainLoopHandle] = setTimeout(mainLoop, 1000 / this[$fps])
    }

    this[$mainLoopHandle] = setTimeout(mainLoop, 0)
  }

  stop () {
    // TODO: stopped?
    if (this[$mainLoopHandle]) {
      // TODO: handle exceptions...
      this[$detach]()

      clearTimeout(this[$mainLoopHandle])
      this[$mainLoopHandle] = null
    }
  }

  // TODO: running flag? or state?

  quit () {
    this[$quitRequested] = true
  }

  [$emit] (event) {
    this[$events].emit(event)
  }

  [$attach] () {
    this[$platformPlugin].attach()

    // TODO: attach managers
    this[$audio][$attach]()
    this[$scene][$attach]()
  }

  [$detach] () {
    // TODO: detach managers
    this[$scene] && this[$scene][$detach]()
    this[$audio][$detach]()

    this[$platformPlugin] && this[$platformPlugin].detach()
  }

  [$destroy] () {
    // TODO: destroy managers
    super[$destroy]()

    this[$scene] && this[$scene][$destroy]()
    this[$audio] && this[$audio][$destroy]()
    this[$audioPlugin] && this[$audioPlugin].destroy()
    this[$platformPlugin] && this[$platformPlugin].destroy()
  }
}

const validatePluginAddonObject = (object) => {
  if (typeof object !== 'object' || typeof object.createInstance !== 'function' ||
      !kPluginTypes.has(object.type)) {
    throw Error('Invalid plugin addon objet.')
  }

  return object
}
