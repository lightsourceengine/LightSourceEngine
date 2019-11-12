/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase, BoxSceneNode, ImageSceneNode, TextSceneNode, RootSceneNode, ImageStoreView } from '../addon'
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
  $image,
  $frame
} from '../util/InternalSymbols'
import { BlurEvent } from '../event/BlurEvent'
import { FocusEvent } from '../event/FocusEvent'
import { performance } from 'perf_hooks'
import { eventBubblePhase } from '../event/eventBubblePhase'

const { now } = performance
const $frameListenersForeground = Symbol.for('frameListenersForeground')
const $frameListenersBackground = Symbol.for('frameListenersBackground')
const sEmptyRafEntry = Object.freeze([0, null])
let sNextFrameRequestId = 1

export class Scene extends SceneBase {
  constructor (stage, displayIndex, width, height, fullscreen) {
    super(stage, displayIndex, width, height, fullscreen)

    if (!this.stage) {
      throw Error('Failed to create Scene (native constructor).')
    }

    this[$displayIndex] = displayIndex
    this[$events] = new EventEmitter()
    this[$activeNode] = null
    this[$image] = new ImageStoreView(this)
    this[$root] = new RootSceneNode(this)
    this[$frameListenersForeground] = []
    this[$frameListenersBackground] = []

    const { style } = this[$root]

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
    const node = new (nodeClass.get(tag) || throwNodeClassNotFound(tag))(this)

    if (!node.scene) {
      throwUninitializedNode(tag)
    }

    return node
  }

  resize (width = 0, height = 0, fullscreen = true) {
    super.resize(width, height, fullscreen)
  }

  requestAnimationFrame (callback) {
    const requestId = sNextFrameRequestId++

    this[$frameListenersForeground].push([requestId, callback])

    return requestId
  }

  cancelAnimationFrame (requestId) {
    removeAnimationFrameListener(requestId, this[$frameListenersForeground])

    // use case: calling cancel in a raf() callback
    removeAnimationFrameListener(requestId, this[$frameListenersBackground])
  }

  [$frame] (delta) {
    if (this[$frameListenersForeground].length) {
      // - background listener list should be empty here
      // - swap background and foreground -> background listeners will be processed right now. any listeners added
      //    during processing will be added to foreground and processed next frame (so user can call raf() in
      //    raf callbacks)

      swapPropValues(this, $frameListenersForeground, $frameListenersBackground)

      for (const [, callback] of this[$frameListenersBackground]) {
        try {
          callback && callback(delta)
        } catch (e) {
          // TODO: exception in user callback.. log?
        }
      }

      this[$frameListenersBackground].length = 0
    }

    super[$frame](delta)
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

const throwUninitializedNode = tag => {
  throw new Error(`Failed to construct node '${tag}'`)
}

const setHasFocus = (node, value) => {
  while (node) {
    node[$hasFocus] = value
    node.waypoint && node.waypoint.sync(node)
    node = node.parent
  }
}

const removeAnimationFrameListener = (requestId, listeners) => {
  const index = listeners.findIndex(value => value[0] === requestId)

  if (index >= 0) {
    // Just clear the listener and preserve the array structure, as the frame may be processing this list right now
    listeners[index] = sEmptyRafEntry
  }
}

const swapPropValues = (obj, prop1, prop2) => {
  const t = obj[prop1]

  obj[prop1] = obj[prop2]
  obj[prop2] = t
}
