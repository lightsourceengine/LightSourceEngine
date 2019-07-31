/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from './Scene'
import { inputEventDispatcher } from './input/inputEventDispatcher'
import bindings from 'bindings'
import { EventEmitter } from 'events'
import { performance } from 'perf_hooks'

const { now } = performance
const $adapter = Symbol.for('adapter')
const $mainLoopHandle = Symbol.for('mainLoopHandle')
const $fps = Symbol.for('fps')
const $attach = Symbol.for('attach')
const $frame = Symbol.for('frame')
const $scene = Symbol.for('scene')

export class Stage {
  constructor () {
    this[$adapter] = null
    this[$mainLoopHandle] = null
    this[$fps] = 60
    this[$scene] = null
  }

  get fps () {
    return this[$fps]
  }

  set fps (value) {
    // 60 or screen refresh rate
    this[$fps] = value
  }

  get keyboard () {
    return this[$adapter].getKeyboard()
  }

  get gamepads () {
    return this[$adapter].getGamepads()
  }

  get displays () {
    return this[$adapter].getDisplays()
  }

  init ({ adapter, audioAdapter } = {}) {
    // TODO: already configured?

    let StageAdapterClass

    if (!adapter) {
      adapter = 'light-source-sdl'
    }

    if (typeof adapter === 'function') {
      StageAdapterClass = adapter
    } else if (typeof adapter === 'string') {
      StageAdapterClass = bindings(adapter).StageAdapter
    } else {
      throw Error()
    }

    this[$adapter] = new StageAdapterClass()

    inputEventDispatcher(this[$adapter], new Map(), new EventEmitter())

    // TODO: move exit handler to stage (?), make configurable (?)

    const errorHandler = obj => {
      if (obj) {
        obj.message && console.log(obj.message)
        obj.stack && console.log(obj.stack)
      }

      process.exit()
    };

    ['SIGINT', 'SIGUSR1', 'SIGUSR2', 'uncaughtException', 'unhandledRejection'].forEach(
      e => process.on(e, errorHandler))

    process.on('exit', () => {
      this.destroy()
      global.gc && global.gc()
    })
  }

  createScene ({ displayIndex, width, height, fullscreen } = {}) {
    if (this[$scene]) {
      throw Error('Stage can only manage 1 scene at a time.')
    }

    // if ! initialized => this.init()

    if (!Number.isInteger(displayIndex)) {
      displayIndex = 0
    }

    if ((width === undefined || width === 0) && (height === undefined || height === 0)) {
      // TODO: get from displays
    } else if (!Number.isInteger(width) && !Number.isInteger(height)) {
      throw Error()
    }

    if (fullscreen === undefined) {
      fullscreen = true
    } else {
      fullscreen = !!fullscreen
    }

    // TODO: options
    // TODO: limit to one scene
    // TODO: auto init
    this[$scene] = new Scene(this, this[$adapter])

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
      if (adapter.processEvents()) {
        const tick = now()
        const delta = tick - lastTick

        lastTick = tick

        scene[$frame](delta)

        this[$mainLoopHandle] = setTimeout(mainLoop, 1000 / this[$fps])
      }
    }

    this[$mainLoopHandle] = setTimeout(mainLoop, 0)
  }

  stop (options) {

  }

  destroy () {

  }
}
