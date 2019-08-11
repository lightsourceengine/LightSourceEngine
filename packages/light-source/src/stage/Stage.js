/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from '../scene/Scene'
import bindings from 'bindings'
import { performance } from 'perf_hooks'
import { addonError } from '../addon'
import { InputManager } from '../input/InputManager'
import { EventEmitter } from '../util/EventEmitter'
import { EventType } from '../event/EventType'

const { now } = performance
const $adapter = Symbol.for('adapter')
const $audioAdapter = Symbol.for('audioAdapter')
const $mainLoopHandle = Symbol.for('mainLoopHandle')
const $fps = Symbol.for('fps')
const $attach = Symbol.for('attach')
const $detach = Symbol.for('detach')
const $frame = Symbol.for('frame')
const $scene = Symbol.for('scene')
const $destroy = Symbol.for('destroy')
const $displays = Symbol.for('displays')
const $exitListener = Symbol.for('exitListener')
const $quitRequested = Symbol.for('quitRequested')
const $input = Symbol.for('input')
const $bind = Symbol.for('bind')
const $unbind = Symbol.for('unbind')
const $events = Symbol.for('events')

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
  }

  get fps () {
    return this[$fps]
  }

  set fps (value) {
    // 60 or screen refresh rate
    this[$fps] = value
  }

  get input () {
    return this[$input]
  }

  get displays () {
    return this[$displays]
  }

  on (id, listener) {
    this[$events].on(id, listener)
  }

  off (id, listener) {
    this[$events].off(id, listener)
  }

  init ({ adapter, audioAdapter } = {}) {
    if (addonError) {
      throw Error('Error loading light-source native addon: ' + addonError.message)
    }

    if (this[$adapter]) {
      throw Error('Stage has already been initialized.')
    }

    let StageAdapterClass

    adapter = adapter || 'light-source-sdl'

    if (typeof adapter === 'function') {
      StageAdapterClass = adapter
    } else if (typeof adapter === 'string') {
      StageAdapterClass = bindings(adapter).StageAdapter
    } else {
      throw Error('adapter must be a module name string or a StageAdapter class.')
    }

    this[$adapter] = new StageAdapterClass()

    try {
      this[$displays] = this[$adapter].getDisplays()
    } catch (e) {
      try {
        adapter.destroy()
      } catch (e) {
        // ignore
      }

      // TODO: throw ?
      throw e
    }

    this[$input][$bind](this)

    process.on('exit', this[$exitListener] = () => {
      if (this[$adapter]) {
        try {
          this[$destroy]()
        } catch (e) {
          console.log('exit: Stage destroy exception:' + e)
        }
      }
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

    if (fullscreen === undefined) {
      fullscreen = true
    } else {
      fullscreen = !!fullscreen
    }

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
      throw Error(`width and height must be integer values.`)
    }

    this[$scene] = new Scene(this, this[$adapter], displayIndex, width, height, fullscreen)

    this[$displays][displayIndex].scene = this[$scene]

    return this[$scene]
  }

  start () {
    // TODO: check already started
    if (this[$mainLoopHandle]) {
      return
    }

    const scene = this[$scene]
    const adapter = this[$adapter]
    let lastTick = now()

    adapter.attach()
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
    if (this[$mainLoopHandle]) {
      // TODO: handle exceptions...
      this[$scene][$detach]()
      this[$adapter].detach()

      clearTimeout(this[$mainLoopHandle])
      this[$mainLoopHandle] = null
    }
  }

  quit () {
    this[$quitRequested] = true
  }

  [$destroy] () {
    const scene = this[$scene]
    const adapter = this[$adapter]
    const audioAdapter = this[$audioAdapter]

    if (this[$exitListener]) {
      process.off('exit', this[$exitListener])
      this[$exitListener] = null
    }

    this[$input][$unbind]()

    clearTimeout(this[$mainLoopHandle])
    this[$mainLoopHandle] = null

    // destroy scene
    if (scene) {
      try {
        scene[$destroy]()
      } catch (e) {
        console.log('Error destroying scene: ' + e)
      }

      this[$scene] = null
    }

    // destroy audio adapter
    if (audioAdapter) {
      try {
        audioAdapter.destroy()
      } catch (e) {
        console.log('Error destroying audio adapter: ' + e)
      }

      this[$audioAdapter] = null
    }

    // destroy stage adapter
    if (adapter) {
      try {
        adapter.destroy()
      } catch (e) {
        console.log('Error destroying stage adapter: ' + e)
      }

      this[$adapter] = null
    }
  }
}
