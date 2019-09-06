/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from '../scene/Scene'
import bindings from 'bindings'
import { performance } from 'perf_hooks'
import { addonError, SDLModuleId } from '../addon'
import { InputManager, InputManager$attach, InputManager$detach } from '../input/InputManager'
import { EventEmitter } from '../util/EventEmitter'
import { EventType } from '../event/EventType'
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
  $resourcePath
} from '../util/InternalSymbols'
import { AudioManager } from '../audio/AudioManager'
import { noexcept, logexcept } from '../util'

const { now } = performance

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
    noexcept(() => adapterInstance.destroy())
    throw Error(`Failed to initialize the StageAdapter instance. Error: ${e.message}`)
  }

  return adapterInstance
}

export class Stage {
  constructor () {
    this[$adapter] = null
    this[$mainLoopHandle] = null
    this[$fps] = 60
    this[$scene] = null
    this[$displays] = []
    this[$exitListener] = null
    this[$input] = new InputManager()
    this[$events] = new EventEmitter([
      EventType.KeyDown,
      EventType.KeyUp,
      EventType.AxisMotion,
      EventType.DeviceConnected,
      EventType.DeviceDisconnected,
      EventType.DeviceButtonDown,
      EventType.DeviceButtonUp,
      EventType.DeviceAxisMotion
    ])
    this[$audio] = new AudioManager(this)
  }

  get fps () {
    return this[$fps]
  }

  set fps (value) {
    // TODO: 60 or screen refresh rate
    this[$fps] = value
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

    this[$resourcePath] = value
    this[$audio][$resourcePath] = value

    if (this[$scene]) {
      this[$scene].resource[$resourcePath] = value
    }
  }

  on (id, listener) {
    this[$events].on(id, listener)
  }

  off (id, listener) {
    this[$events].off(id, listener)
  }

  init ({ adapter, audioAdapter } = {}) {
    if (addonError) {
      throw Error(`Error loading light-source native addon: ${addonError.message}`)
    }

    if (this[$adapter]) {
      throw Error('Stage has already been initialized.')
    }

    this[$adapter] = createStageAdapter(validateAdapterConfig(adapter))

    try {
      this[$displays] = this[$adapter].getDisplays()
    } catch (e) {
      console.log(`Failed to get displays. Error: ${e.message}`)
      this[$displays] = []
    }

    this[$audio][$init](audioAdapter)

    InputManager$attach(this[$input], this)

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

    this[$scene] = new Scene(this, this[$adapter], displayIndex, width, height, fullscreen)

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
    let lastTick = now()

    // TODO: handle exceptions

    adapter.attach()
    InputManager$attach(this[$input], this)
    audio[$attach]()
    scene[$attach]()

    const mainLoop = () => {
      const tick = now()
      const delta = tick - lastTick

      if (!adapter.processEvents() || this[$quitRequested]) {
        return
      }

      lastTick = tick

      scene[$frame](delta)

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
      InputManager$detach(this[$input])
      this[$adapter].detach()

      clearTimeout(this[$mainLoopHandle])
      this[$mainLoopHandle] = null
    }
  }

  // TODO: running flag? or state?

  quit () {
    this[$quitRequested] = true
  }

  [$destroy] () {
    const scene = this[$scene]
    const adapter = this[$adapter]
    const audio = this[$audio]
    const input = this[$input]

    if (this[$mainLoopHandle]) {
      clearTimeout(this[$mainLoopHandle])
      this[$mainLoopHandle] = null
    }

    if (input) {
      logexcept(() => InputManager$detach(input), 'Failed to unbind InputManager. Error: ')
    }

    // TODO: destroy resource manager?

    // destroy scene
    if (scene) {
      logexcept(() => scene[$destroy](), 'Failed to destroy Scene. Error: ')
      this[$scene] = null
    }

    // destroy audio adapter
    if (audio) {
      logexcept(() => audio[$destroy](), 'Failed to destroy AudioManager. Error: ')
      this[$audio] = null
    }

    // destroy stage adapter
    if (adapter) {
      logexcept(() => adapter.destroy(), 'Failed to destroy StageAdapter. Error: ')
      this[$adapter] = null
    }

    if (this[$exitListener]) {
      process.off('exit', this[$exitListener])
      this[$exitListener] = null
    }
  }
}
