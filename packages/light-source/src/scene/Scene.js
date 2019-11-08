/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase, BoxSceneNode, ImageSceneNode, TextSceneNode, ImageStoreView } from '../addon'
import { EventEmitter } from '../util/EventEmitter'
import {
  $width,
  $height,
  $fullscreen,
  $stage,
  $root,
  $destroy,
  $displayIndex,
  $activeNode,
  $destroying,
  $events,
  $hasFocus,
  $emit,
  $image
} from '../util/InternalSymbols'
import { BlurEvent } from '../event/BlurEvent'
import { FocusEvent } from '../event/FocusEvent'
import { performance } from 'perf_hooks'
import { eventBubblePhase } from '../event/eventBubblePhase'

const { now } = performance

export class Scene extends SceneBase {
  constructor (stage, displayIndex, width, height, fullscreen) {
    super(stage, displayIndex, width, height, fullscreen)

    this[$displayIndex] = displayIndex
    this[$events] = new EventEmitter()
    this[$activeNode] = null
    this[$image] = new ImageStoreView(this)

    const { style } = this.root

    style.position = 'absolute'
    style.left = 0
    style.top = 0
    style.right = 0
    style.bottom = 0
    style.fontSize = 16
    style.backgroundColor = 'black'
  }

  get root () {
    return this[$root]
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

  get image () {
    return this[$image]
  }

  on (id, listener) {
    this[$events].on(id, listener)
  }

  off (id, listener) {
    this[$events].off(id, listener)
  }

  get activeNode () {
    return this[$activeNode]
  }

  set activeNode (value) {
    let activeNode = this[$activeNode]

    if ((activeNode === value) || (!activeNode && !value)) {
      return
    }

    const timestamp = now()
    const { stage } = this

    if (activeNode) {
      setHasFocus(activeNode, false)
      eventBubblePhase(stage, this, new BlurEvent(timestamp))
    }

    this[$activeNode] = activeNode = value || null

    if (activeNode) {
      setHasFocus(activeNode, true)
      eventBubblePhase(stage, this, new FocusEvent(timestamp))
    }
  }

  // TODO: add refreshRate

  // TODO: add vsync

  createNode (tag) {
    return new (nodeClass.get(tag) || throwNodeClassNotFound(tag))(this)
  }

  resize (width = 0, height = 0, fullscreen = true) {
    super.resize(width, height, fullscreen)
  }

  [$emit] (event) {
    this[$events].emit(event)
  }

  [$destroy] () {
    if (this[$events]) {
      this[$emit]({ type: $destroying })
      this[$events] = null
    }

    super[$destroy]()

    this[$stage] = null
    this[$activeNode] = null
    this[$image] = null
  }
}

const nodeClass = new Map([
  ['img', ImageSceneNode],
  ['div', BoxSceneNode],
  ['box', BoxSceneNode],
  ['text', TextSceneNode]
])

const throwNodeClassNotFound = tag => {
  throw new Error(`'${tag}' is not a valid scene node tag.`)
}

const setHasFocus = (node, value) => {
  while (node) {
    node[$hasFocus] = value
    node.waypoint && node.waypoint.sync(node)
    node = node.parent
  }
}
