/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Scene } from './Scene'
import bindings from 'bindings'

const adapter = Symbol.for('adapter')
const mainLoopHandle = Symbol.for('mainLoopHandle')
const fps = Symbol.for('fps')
const attach = Symbol.for('attach')

export class Stage {
  constructor () {
    this[adapter] = null
    this[mainLoopHandle] = null
    this[fps] = 60

    this.scene = new Scene(this)
  }

  get fps () {
    return this[fps]
  }

  set fps (value) {
    // 60 or screen refresh rate
    this[fps] = value
  }

  configure (options = {}) {
    // TODO: already configured?

    const module = options.StageAdapter === 'function' ? options : bindings(options.StageAdapter || 'light-source-sdl')

    this[adapter] = new module.StageAdapter()
  }

  start () {
    if (this[mainLoopHandle]) {
      return
    }

    this.configure()

    this.scene[attach]()

    const mainLoop = () => {
      if (this[adapter].processEvents()) {
        this[mainLoopHandle] = setTimeout(mainLoop, 1000 / this[fps])
      }
    }

    this[mainLoopHandle] = setTimeout(mainLoop, 0)
  }

  stop (options) {

  }

  destroy () {

  }
}
