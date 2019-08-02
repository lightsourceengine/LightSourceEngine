/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase, BoxSceneNode, ImageSceneNode, TextSceneNode } from './addon-light-source'
import { Style } from './Style'
import EventEmitter from 'events'

const $width = Symbol.for('width')
const $height = Symbol.for('height')
const $fullscreen = Symbol.for('fullscreen')
const $stage = Symbol.for('stage')
const $root = Symbol.for('root')
const $resource = Symbol.for('resource')
const $destroy = Symbol.for('destroy')
const $events = Symbol.for('events')
const $displayIndex = Symbol.for('displayIndex')

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
  constructor (stage, stageAdapter, displayIndex, width, height, fullscreen) {
    super(stageAdapter, displayIndex, width, height, fullscreen)

    this[$stage] = stage
    this[$events] = new EventEmitter()
    this[$displayIndex] = displayIndex

    this.root.style = new Style({
      position: 'absolute',
      left: 0,
      top: 0,
      right: 0,
      bottom: 0,
      fontSize: 16
    })
  }

  get stage () {
    return this[$stage]
  }

  get root () {
    return this[$root]
  }

  get resource () {
    return this[$resource]
  }

  get fullscreen () {
    return this[$fullscreen]
  }

  get width () {
    return this[$width]
  }

  get height () {
    return this[$height]
  }

  get displayIndex () {
    return this[$displayIndex]
  }

  // TODO: add refreshRate

  // TODO: add vsync

  createNode (tag) {
    return new (nodeClass.get(tag) || nodeClassNotFound(tag))(this)
  }

  resize (width = 0, height = 0, fullscreen = true) {
    super.resize(width, height, fullscreen)
  }

  [$destroy] () {
    if (this[$events]) {
      this[$events].emit('destroying')
      this[$events] = null
    }

    this[$stage] = null

    super[$destroy]()
  }
}
