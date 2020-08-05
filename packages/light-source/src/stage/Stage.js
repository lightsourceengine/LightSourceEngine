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
  $emit
} from '../util/InternalSymbols'
import { AudioManager } from '../audio/AudioManager'
import { isNumber, logexcept } from '../util'

const { now } = performance

const $plugins = Symbol.for('plugins')

const kPluginTypeAudio = 'audio'
const kPluginTypePlatform = 'platform'

const kNullPlatformPlugin = {
  capabilities: Object.freeze({
    displays: []
  })
}

export class Stage extends StageBase {
  constructor () {
    super()
    this[$plugins] = new Map()

    this[$mainLoopHandle] = null
    this[$fps] = 60
    this[$scene] = null
    this[$input] = new InputManager(this)
    this[$audio] = new AudioManager(this)
    this[$events] = new EventEmitter()

    process.on('exit', () => {
      logexcept(() => this[$destroy](), 'exit: Stage destroy exception: ')
      global.gc && global.gc()
    })
  }

  get capabilities () {
    return (getPlatformPlugin(this) || kNullPlatformPlugin).capabilities
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
    if (this.hasPlugin(kPluginTypePlatform) || this.hasPlugin(kPluginTypeAudio)) {
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
    } else if (typeof plugin === 'function') {
      Plugin = plugin
    } else {
      throw Error('plugin argument must be a path to a .node addon or a Plugin class.')
    }

    const { type } = Plugin

    if (!(type === kPluginTypeAudio || type === kPluginTypePlatform)) {
      throw Error(`Unknown plugin type: '${type}'`)
    }

    if (this.hasPlugin(type)) {
      throw Error(`Plugin '${type}' has already loaded.`)
    }

    let instance

    try {
      instance = Plugin.createInstance()
    } catch (e) {
      throw Error(`Failed to create Plugin instance. Error: ${e.message}`)
    }

    this[$plugins].set(type, instance)

    if (type === kPluginTypePlatform) {
      // this[$input][$init](stageAdapter, gameControllerDb)
    } else if (type === kPluginTypeAudio) {
      // this[$audio][$init](audioAdapter)
    }
  }

  hasPlugin (pluginType) {
    return this[$plugins].has(pluginType)
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

  createScene ({ displayIndex, width, height, fullscreen } = {}) {
    if (!this.hasPlugin(kPluginTypePlatform)) {
      this.init()
    }

    if (this[$scene]) {
      throw Error('Stage can only manage 1 scene at a time.')
    }

    const { capabilities } = this

    if (!Number.isInteger(displayIndex)) {
      displayIndex = 0
    }

    if (displayIndex < 0 || displayIndex >= capabilities.displays.length) {
      throw Error(`Invalid displayIndex ${displayIndex}.`)
    }

    fullscreen = (fullscreen === undefined) || (!!fullscreen)

    if ((width === undefined || width === 0) && (height === undefined || height === 0)) {
      if (fullscreen) {
        const { defaultMode } = capabilities.displays[displayIndex]

        width = defaultMode.width
        height = defaultMode.height
      } else {
        width = 1280
        height = 720
      }
    } else if (Number.isInteger(width) && Number.isInteger(height)) {
      width = width >> 0
      height = height >> 0

      if (fullscreen) {
        const i = capabilities.displays[displayIndex].modes.findIndex(mode => mode.width === width && mode.height === height)

        if (i === -1) {
          throw Error(`Fullscreen size ${width}x${height} is not available on this system.`)
        }
      }
    } else {
      throw Error('width and height must be integer values.')
    }

    let adapter

    try {
      adapter = getPlatformPlugin(this).createSceneAdapter({ displayIndex, width, height, fullscreen })
    } catch (e) {
      throw Error(e.message)
    }

    let scene

    try {
      scene = new Scene(this, adapter)
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

    const graphicsPlugin = getPlatformPlugin(this)
    const scene = this[$scene]
    let lastTick = now()

    this[$attach]()

    const mainLoop = () => {
      const tick = now()

      if (!graphicsPlugin.processEvents() || this[$quitRequested]) {
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
    getPlugins(this).forEach(plugin => {
      plugin && plugin.attach()
    })

    // TODO: attach managers
    this[$scene][$attach]()
  }

  [$detach] () {
    // TODO: detach managers
    this[$scene][$detach]()

    getPlugins(this).reduceRight((_, plugin) => {
      plugin && plugin.detach()
    })
  }

  [$destroy] () {
    // TODO: destroy managers

    if (this[$scene]) {
      this[$scene][$destroy]()
    }

    super[$destroy]()

    getPlugins(this).reduceRight((_, plugin) => {
      plugin && plugin.destroy()
    })
  }
}

const getPlugins = (stage) => {
  const plugins = stage[$plugins]

  return [
    plugins.get(kPluginTypePlatform),
    plugins.get(kPluginTypeAudio)
  ]
}

const getPlatformPlugin = (stage) => stage[$plugins].get(kPluginTypePlatform)
