/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase } from './addon-light-source'

const attach = Symbol.for('attach')
const detach = Symbol.for('detach')

export class Scene extends SceneBase {
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
    super.resize(width, height, fullscreen)
  }

  [attach] () {
    super[attach]()
  }

  [detach] () {
    super[detach]()
  }
}
