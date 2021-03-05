/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { CScene } from '../addon/index.js'
import { BoxSceneNode, ImageSceneNode, TextSceneNode, RootSceneNode } from './SceneNode.js'
import { createAttachedEvent, createDestroyedEvent, createDestroyingEvent, createDetachedEvent } from '../event/index.js'
import { EventName } from '../event/EventName.js'
import { EventTarget } from '../event/EventTarget.js'
import { ImageManager } from '../image/ImageManager.js'

const kEmptyFrameListener = Object.freeze([0, null])
let sFrameRequestId = 0

export class Scene extends EventTarget {
  _native = null
  _context = null
  _root = null
  _stage = null
  _activeNode = null
  _fgFrameListeners = []
  _bgFrameListeners = []
  _attached = false
  _imageManager = new ImageManager()

  constructor (stage, config) {
    super([
      EventName.onAttached,
      EventName.onDetached,
      EventName.onDestroying,
      EventName.onDestroyed
    ])

    this._stage = stage
    this._context = stage.system.$createGraphicsContext(config)
    this._native = new CScene(stage.$native, stage.font.$native, this._imageManager.$native, this._context)
    this._root = new RootSceneNode(this)
    const { style } = this._root

    style.backgroundColor = 'black'
    style.top = 0
    style.right = 0
    style.bottom = 0
    style.left = 0
    style.position = 'absolute'

    this._native.setRoot(this._root.$native)

    if (typeof config?.title === 'string') {
      this.title = config.title
    }
  }

  get stage () {
    return this._stage
  }

  get root () {
    return this._root
  }

  get image () {
    return this._imageManager
  }

  isAttached () {
    return this._attached
  }

  get fullscreen () {
    return this._context.isFullscreen()
  }

  get width () {
    return this._context.getWidth()
  }

  get height () {
    return this._context.getHeight()
  }

  get displayIndex () {
    return this._context.getDisplayIndex()
  }

  get title () {
    return this._context.getTitle()
  }

  set title (value) {
    this._context.setTitle(value)
  }

  get activeNode () {
    return this._activeNode
  }

  $setActiveNode (node) {
    this._activeNode = node
  }

  // TODO: add refreshRate

  // TODO: add vsync

  createNode (tag) {
    return new (nodeClass.get(tag) || throwNodeClassNotFound(tag))(this)
  }

  resize (width, height, fullscreen = true) {
    this._context.resize(width, height, fullscreen)
  }

  requestAnimationFrame (callback) {
    this._fgFrameListeners.push([++sFrameRequestId, callback])

    return sFrameRequestId
  }

  cancelAnimationFrame (requestId) {
    removeAnimationFrameListener(requestId, this._fgFrameListeners)

    // use case: calling cancel in a raf() callback
    removeAnimationFrameListener(requestId, this._bgFrameListeners)
  }

  $frame (tick, lastTick) {
    if (this._fgFrameListeners.length) {
      // - background listener list should be empty here
      // - swap background and foreground -> background listeners will be processed right now. any listeners added
      //    during processing will be added to foreground and processed next frame (so user can call raf() in
      //    raf callbacks)

      [this._bgFrameListeners, this._fgFrameListeners] =
        [this._fgFrameListeners, this._bgFrameListeners]

      for (const [/* handle */, callback] of this._bgFrameListeners) {
        try {
          callback && callback(tick, lastTick)
        } catch (e) {
          // TODO: exception in user callback.. log?
        }
      }

      this._bgFrameListeners.length = 0
    }

    this._native.render(tick, lastTick)
  }

  /**
   * @ignore
   */
  $attach () {
    if (this._attached) {
      return
    }

    this._native.attach()

    this._attached = true
    this.dispatchEvent(createAttachedEvent(this))
  }

  /**
   * @ignore
   */
  $detach () {
    if (!this._attached) {
      return
    }

    this._native.detach()

    this._attached = false
    this.dispatchEvent(createDetachedEvent(this))
  }

  /**
   * @ignore
   */
  $destroy () {
    if (!this._stage) {
      return
    }

    this.$detach()

    this.dispatchEvent(createDestroyingEvent(this))

    this._stage._scenes.delete(this.displayIndex)
    if (this._stage.$scene === this) {
      this._stage.$scene = null
    }

    this._fgFrameListeners = []
    this._fgFrameListeners = []
    this._activeNode = null
    this._root?.destroy()
    this._root = null
    this._native?.destroy()
    this._native = null
    this._context = null
    this._stage = null

    this.dispatchEvent(createDestroyedEvent(this))

    // TODO: clear event target listeners?
  }

  /**
   * @ignore
   */
  get $native () {
    return this._native
  }
}

const nodeClass = new Map([
  ['box', BoxSceneNode],
  ['div', BoxSceneNode],
  ['img', ImageSceneNode],
  ['text', TextSceneNode]
])

const throwNodeClassNotFound = tag => {
  throw new Error(`'${tag}' is not a valid scene node tag.`)
}

const removeAnimationFrameListener = (requestId, listeners) => {
  const index = listeners.findIndex(value => value[0] === requestId)

  if (index >= 0) {
    // Just clear the listener and preserve the array structure, as the frame may be processing this list right now
    listeners[index] = kEmptyFrameListener
  }
}
