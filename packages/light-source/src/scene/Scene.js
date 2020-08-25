/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { performance } from 'perf_hooks'
import { SceneBase, BoxSceneNode, ImageSceneNode, TextSceneNode, LinkSceneNode } from '../addon'
import { EventEmitter } from '../util/EventEmitter'
import { BlurEvent } from '../event/BlurEvent'
import { FocusEvent } from '../event/FocusEvent'
import { eventBubblePhase } from '../event/eventBubblePhase'
import { absoluteFill } from '../style/absoluteFill'

const { now } = performance
const $hasFocus = Symbol.for('hasFocus')
const $activeNode = Symbol.for('activeNode')
const sEmptyRafEntry = Object.freeze([0, null])
let sFrameRequestId = 0

export class Scene extends SceneBase {
  _emitter = new EventEmitter()
  _frameListenersForeground = []
  _frameListenersBackground = []

  constructor (stage, platform, config) {
    super(stage, createGraphicsContext(stage, platform, config))

    this[$activeNode] = null

    Object.assign(this.root.style, {
      ...absoluteFill,
      backgroundColor: 'black'
    })
  }

  get fullscreen () {
    return this._graphicsContext.fullscreen
  }

  get width () {
    return this._graphicsContext.width
  }

  get height () {
    return this._graphicsContext.height
  }

  get displayIndex () {
    return this._graphicsContext.displayIndex
  }

  get title () {
    return this._graphicsContext.title
  }

  set title (value) {
    this._graphicsContext.title = value
  }

  on (id, listener) {
    this._emitter.on(id, listener)
  }

  off (id, listener) {
    this._emitter.off(id, listener)
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

  resize (width, height, fullscreen = true) {
    this._graphicsContext.resize(width, height, fullscreen)
  }

  requestAnimationFrame (callback) {
    this._frameListenersForeground.push([++sFrameRequestId, callback])

    return sFrameRequestId
  }

  cancelAnimationFrame (requestId) {
    removeAnimationFrameListener(requestId, this._frameListenersForeground)

    // use case: calling cancel in a raf() callback
    removeAnimationFrameListener(requestId, this._frameListenersBackground)
  }

  $frame (tick, lastTick) {
    if (this._frameListenersForeground.length) {
      // - background listener list should be empty here
      // - swap background and foreground -> background listeners will be processed right now. any listeners added
      //    during processing will be added to foreground and processed next frame (so user can call raf() in
      //    raf callbacks)

      [this._frameListenersBackground, this._frameListenersForeground] =
        [this._frameListenersForeground, this._frameListenersBackground]

      for (const [/* handle */, callback] of this._frameListenersBackground) {
        try {
          callback && callback(tick, lastTick)
        } catch (e) {
          // TODO: exception in user callback.. log?
        }
      }

      this._frameListenersBackground.length = 0
    }

    super.$frame(tick, lastTick)
  }

  $emit (event) {
    this._emitter.emit(event)
  }

  $attach () {
    this._graphicsContext.attach()

    super.$attach()
  }

  $detach () {
    super.$detach()

    this._graphicsContext.detach()
  }

  $destroy () {
    super.$destroy()

    this[$activeNode] = null
  }
}

const nodeClass = new Map([
  ['box', BoxSceneNode],
  ['div', BoxSceneNode],
  ['img', ImageSceneNode],
  ['link', LinkSceneNode],
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

const removeAnimationFrameListener = (requestId, listeners) => {
  const index = listeners.findIndex(value => value[0] === requestId)

  if (index >= 0) {
    // Just clear the listener and preserve the array structure, as the frame may be processing this list right now
    listeners[index] = sEmptyRafEntry
  }
}

const createGraphicsContext = (stage, platform, { displayIndex, width, height, fullscreen }) => {
  const { capabilities } = stage

  if (!Number.isInteger(displayIndex)) {
    displayIndex = 0
  }

  if (displayIndex < 0 || displayIndex >= capabilities.displays.length) {
    throw Error(`Invalid displayIndex ${displayIndex}.`)
  }

  fullscreen = (fullscreen === undefined) || (!!fullscreen)

  if ((width === undefined || width === 0) && (height === undefined || height === 0)) {
    if (fullscreen) {
      const { defaultMode } = capabilities.displays[displayIndex]

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
      const i = capabilities.displays[displayIndex].modes.findIndex(mode => mode.width === width && mode.height === height)

      if (i === -1) {
        throw Error(`Fullscreen size ${width}x${height} is not available on this system.`)
      }
    }
  } else {
    throw Error('width and height must be integer values.')
  }

  return platform.createGraphicsContext({ displayIndex, width, height, fullscreen })
}
