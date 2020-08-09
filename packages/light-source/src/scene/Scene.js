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
import {
  $destroy,
  $activeNode,
  $destroying,
  $events,
  $hasFocus,
  $emit,
  $frame,
  $attach,
  $detach
} from '../util/InternalSymbols'
import { absoluteFill } from '../style/absoluteFill'

const { now } = performance
const $frameListenersForeground = Symbol.for('frameListenersForeground')
const $frameListenersBackground = Symbol.for('frameListenersBackground')
const $graphicsContext = Symbol.for('graphicsContext')
const sEmptyRafEntry = Object.freeze([0, null])
let sFrameRequestId = 0

export class Scene extends SceneBase {
  constructor (stage, platform, config) {
    super(stage, createGraphicsContext(stage, platform, config))

    this[$events] = new EventEmitter()
    this[$activeNode] = null
    this[$frameListenersForeground] = []
    this[$frameListenersBackground] = []

    const { style } = this.root

    Object.assign(style, {
      ...absoluteFill,
      backgroundColor: 'black'
    })
  }

  get fullscreen () {
    return this[$graphicsContext].fullscreen
  }

  get width () {
    return this[$graphicsContext].width
  }

  get height () {
    return this[$graphicsContext].height
  }

  get displayIndex () {
    return this[$graphicsContext].displayIndex
  }

  get title () {
    return this[$graphicsContext].title
  }

  set title (value) {
    this[$graphicsContext].title = value
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

  resize (width, height, fullscreen = true) {
    this[$graphicsContext].resize(width, height, fullscreen)
  }

  requestAnimationFrame (callback) {
    this[$frameListenersForeground].push([++sFrameRequestId, callback])

    return sFrameRequestId
  }

  cancelAnimationFrame (requestId) {
    removeAnimationFrameListener(requestId, this[$frameListenersForeground])

    // use case: calling cancel in a raf() callback
    removeAnimationFrameListener(requestId, this[$frameListenersBackground])
  }

  [$frame] (tick, lastTick) {
    if (this[$frameListenersForeground].length) {
      // - background listener list should be empty here
      // - swap background and foreground -> background listeners will be processed right now. any listeners added
      //    during processing will be added to foreground and processed next frame (so user can call raf() in
      //    raf callbacks)

      swapPropValues(this, $frameListenersForeground, $frameListenersBackground)

      for (const [/* handle */, callback] of this[$frameListenersBackground]) {
        try {
          callback && callback(tick, lastTick)
        } catch (e) {
          // TODO: exception in user callback.. log?
        }
      }

      this[$frameListenersBackground].length = 0
    }

    super[$frame](tick, lastTick)
  }

  [$emit] (event) {
    this[$events].emit(event)
  }

  [$attach] () {
    this[$graphicsContext].attach()

    super[$attach]()
  }

  [$detach] () {
    super[$detach]()

    this[$graphicsContext].detach()
  }

  [$destroy] () {
    super[$destroy]()

    this[$activeNode] = null

    if (this[$events]) {
      this[$emit]({ type: $destroying })
      this[$events] = null
    }
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

const swapPropValues = (obj, prop1, prop2) => {
  const t = obj[prop1]

  obj[prop1] = obj[prop2]
  obj[prop2] = t
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
