/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase, BoxSceneNode, ImageSceneNode, TextSceneNode } from './addon-light-source'
import { Style } from './Style'

const $attach = Symbol.for('attach')
const $detach = Symbol.for('detach')
const nodeClass = new Map([
  ['img', ImageSceneNode],
  ['div', BoxSceneNode],
  ['box', BoxSceneNode],
  ['text', TextSceneNode]
])
const nodeClassNotFound = tag => {
  throw new Error(`'${tag}' is not a valid scene node tag.`)
}

export class Scene extends SceneBase {
  constructor (stage) {
    super(stage)

    this.root.style = new Style({
      position: 'absolute',
      left: 0,
      top: 0,
      right: 0,
      bottom: 0
    })
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
    return new (nodeClass.get(tag) || nodeClassNotFound(tag))(this)
  }

  resize (width = 0, height = 0, fullscreen = true) {
    super.resize(width, height, fullscreen)
  }

  [$attach] () {
    super[$attach]()
  }

  [$detach] () {
    super[$detach]()
  }
}
