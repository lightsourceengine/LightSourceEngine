/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from '../scene/Scene'
import bindings from 'bindings'
import { StageBase, addonError, SDLModuleId, FontStoreView } from '../addon'
import { performance } from 'perf_hooks'
import { join } from 'path'

import { InputManager } from '../input/InputManager'
import { EventEmitter } from '../util/EventEmitter'
import {
  $adapter,
  $mainLoopHandle,
  $fps,
  $attach,
  $detach,
  $destroy,
  $frame,
  $displays,
  $exitListener,
  $quitRequested,
  $input,
  $events,
  $scene,
  $audio,
  $init,
  $resourcePath,
  $emit,
  $setResourcePath,
  $font, $processEvents
} from '../util/InternalSymbols'
import { AudioManager } from '../audio/AudioManager'
import { isNumber, logexcept } from '../util'

const { now } = performance

export class Stage extends StageBase {
  constructor () {
    super()
    this[$mainLoopHandle] = null
    this[$fps] = 60
    this[$scene] = null
    this[$displays] = []
    this[$exitListener] = null
    this[$input] = new InputManager(this)
    this[$audio] = new AudioManager(this)
    this[$events] = new EventEmitter()
    this[$font] = new FontStoreView(this)
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

  get font () {
    return this[$font]
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

  get displays () {
    return this[$displays]
  }

  /**
   * Local file directory used when resolving the resource host name of a file URI.
   *
   * Resource loading, including images, audio and fonts, accepts a file URI with a resource hostname
   * ('file://resource/image.png'). The file://resource prefix is replaced with resourcePath to load the
   * local file.
   *
   * @property resourcePath
   * @returns {string} an absolute or relative file path. If not set, an empty string is returned.
   */

  get resourcePath () {
    return this[$resourcePath]
  }

  set resourcePath (value) {
    if (!value) {
      value = ''
    } else if (typeof value !== 'string') {
      throw Error(`resourcePath must be a string. Got: ${value}`)
    }

    if (value) {
      value = join(value)
    }

    this[$audio][$setResourcePath](value)

    this[$resourcePath] = value
  }

  on (id, listener) {
    this[$events].on(id, listener)
  }

  off (id, listener) {
    this[$events].off(id, listener)
  }

  init ({ adapter, audioAdapter, gameControllerDb } = {}) {
    if (addonError) {
      throw Error(`Error loading light-source native addon: ${addonError.message}`)
    }

    if (this[$adapter]) {
      throw Error('Stage has already been initialized.')
    }

    const stageAdapter = createStageAdapter(validateAdapterConfig(adapter))

    this[$adapter] = stageAdapter

    try {
      this[$displays] = stageAdapter.getDisplays()
    } catch (e) {
      console.log(`Failed to get displays. Error: ${e.message}`)
      this[$displays] = []
    }

    this[$audio][$init](audioAdapter)
    this[$input][$init](stageAdapter, gameControllerDb)

    process.on('exit', this[$exitListener] = () => {
      logexcept(() => this[$destroy](), 'exit: Stage destroy exception: ')
      global.gc && global.gc()
    })
  }

  createScene ({ displayIndex, width, height, fullscreen } = {}) {
    if (!this[$adapter]) {
      this.init()
    }

    if (this[$scene]) {
      throw Error('Stage can only manage 1 scene at a time.')
    }

    if (!Number.isInteger(displayIndex)) {
      displayIndex = 0
    } else {
      displayIndex = displayIndex >> 0
    }

    if (displayIndex < 0 || displayIndex >= this.displays.length) {
      throw Error(`Invalid displayIndex ${displayIndex}.`)
    }

    fullscreen = (fullscreen === undefined) || (!!fullscreen)

    if ((width === undefined || width === 0) && (height === undefined || height === 0)) {
      if (fullscreen) {
        const { defaultMode } = this.displays[displayIndex]

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
        const i = this.displays[displayIndex].modes.findIndex(mode => mode.width === width && mode.height === height)

        if (i === -1) {
          throw Error(`Fullscreen size ${width}x${height} is not available on this system.`)
        }
      }
    } else {
      throw Error('width and height must be integer values.')
    }

    this[$scene] = new Scene(this, displayIndex, width, height, fullscreen)

    this[$displays][displayIndex].scene = this[$scene]

    return this[$scene]
  }

  start () {
    // TODO: initialized?
    // TODO: check already started
    if (this[$mainLoopHandle]) {
      return
    }

    const scene = this[$scene]
    const adapter = this[$adapter]
    const audio = this[$audio]
    const input = this[$input]
    let lastTick = now()

    // TODO: handle exceptions

    adapter.attach()

    input[$attach]()
    audio[$attach]()
    scene[$attach]()

    const mainLoop = () => {
      const tick = now()

      // TODO: clean up adapter interface
      this[$processEvents]()

      if (!adapter.processEvents() || this[$quitRequested]) {
        // TODO: partial destroy here? .. so stage can be restarted?
        this[$destroy]()
        return
      }

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
      this[$scene][$detach]()
      this[$audio][$detach]()
      this[$input][$detach]()
      this[$adapter].detach()

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

  [$destroy] () {
    const scene = this[$scene]
    const adapter = this[$adapter]
    const audio = this[$audio]
    const input = this[$input]
    const mainLoopHandle = this[$mainLoopHandle]
    const exitListener = this[$exitListener]

    mainLoopHandle && clearTimeout(mainLoopHandle)
    this[$mainLoopHandle] = null

    exitListener && process.off('exit', exitListener)
    this[$exitListener] = null

    if (scene) {
      logexcept(() => scene[$destroy](), 'Failed to destroy Scene. Error: ')
      this[$displays][scene.displayIndex].scene = null
      this[$scene] = null
    }

    logexcept(() => input[$destroy](), 'Failed to unbind InputManager. Error: ')
    logexcept(() => audio[$destroy](), 'Failed to destroy AudioManager. Error: ')

    logexcept(() => adapter && adapter.destroy(), 'Failed to destroy StageAdapter. Error: ')
    this[$adapter] = null

    this[$font] = null
  }
}

const validateAdapterConfig = (adapter) => {
  if (typeof adapter === 'function' || typeof adapter === 'string') {
    return adapter
  } else if (!adapter) {
    return SDLModuleId
  }

  throw Error('adapter must be a module name string or a StageAdapter class.')
}

const createStageAdapter = (adapter) => {
  let StageAdapter

  if (typeof adapter === 'string') {
    try {
      StageAdapter = bindings(adapter).StageAdapter
    } catch (e) {
      throw Error(`Module ${adapter} failed to load. Error: ${e.message}`)
    }

    if (typeof StageAdapter !== 'function') {
      throw Error(`Module ${adapter} does not contain an StageAdapter class.`)
    }
  } else {
    StageAdapter = adapter
  }

  let adapterInstance

  try {
    adapterInstance = new StageAdapter()
  } catch (e) {
    throw Error(`Failed to construct the StageAdapter instance. Error: ${e.message}`)
  }

  try {
    adapterInstance.attach()
  } catch (e) {
    logexcept(() => adapterInstance.destroy())
    throw Error(`Failed to initialize the StageAdapter instance. Error: ${e.message}`)
  }

  return adapterInstance
}
