/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

const attach = Symbol.for('attach')
const detach = Symbol.for('detach')
const adapter = Symbol.for('adapter')

export class Scene {
  constructor (stage) {
    this.stage = stage
  }

  get title () {
    return ''
  }

  set title (value) {

  }

  get fullscreen () {
    return false
  }

  get width () {

  }

  get height () {

  }

  createNode (tag) {

  }

  resize (width = 0, height = 0, fullscreen = true) {
    this[adapter].resize(width, height, fullscreen)
  }

  [attach] () {
    this[adapter] = this.stage[adapter].createSceneAdapter(0)
    this[adapter].attach()
  }

  [detach] () {
    this[adapter].detach()
  }
}
