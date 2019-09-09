/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase, BoxSceneNode, ImageSceneNode, TextSceneNode } from '../addon'
import { Style } from '../style/Style'
import { EventEmitter } from '../util/EventEmitter'
import {
  $width,
  $height,
  $fullscreen,
  $stage,
  $root,
  $resource,
  $destroy,
  $displayIndex,
  $activeNode,
  $destroying,
  $events,
  $hasFocus,
  $resourcePath,
  $emit
} from '../util/InternalSymbols'
import { BlurEvent } from '../event/BlurEvent'
import { FocusEvent } from '../event/FocusEvent'
import { performance } from 'perf_hooks'
import { eventBubblePhase } from '../event/eventBubblePhase'

const { now } = performance

export class Scene extends SceneBase {
  constructor (stage, stageAdapter, displayIndex, width, height, fullscreen) {
    super(stageAdapter, displayIndex, width, height, fullscreen)

    this[$stage] = stage
    this[$displayIndex] = displayIndex
    this[$events] = new EventEmitter()
    this[$activeNode] = null
    this[$resource][$resourcePath] = stage.resourcePath

    this.root.style = new Style({
      position: 'absolute',
      left: 0,
      top: 0,
      right: 0,
      bottom: 0,
      fontSize: 16,
      backgroundColor: 'black'
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

    if (activeNode) {
      setHasFocus(activeNode, false)
      eventBubblePhase(this[$stage], this, new BlurEvent(timestamp))
    }

    this[$activeNode] = activeNode = value || null

    if (activeNode) {
      setHasFocus(activeNode, true)
      eventBubblePhase(this[$stage], this, new FocusEvent(timestamp))
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
