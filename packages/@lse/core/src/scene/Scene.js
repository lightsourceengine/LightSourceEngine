/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase } from '../addon/index.js'
import { BoxSceneNode, ImageSceneNode, TextSceneNode, LinkSceneNode, RootSceneNode } from './SceneNode.js'
import { EventEmitter } from '../util/index.js'
import { AttachedEvent, DestroyedEvent, DestroyingEvent, DetachedEvent, EventNames } from '../event/index.js'
import { fileURLToPath } from 'url'
import { join, dirname } from 'path'

const kEmptyFrameListener = Object.freeze([0, null])
let sFrameRequestId = 0

export class Scene extends SceneBase {
  _activeNode = null
  _emitter = new EventEmitter([
    EventNames.attached,
    EventNames.detached,
    EventNames.destroying,
    EventNames.destroyed
  ])

  _stage = null
  _root = null
  _fonts = []
  _graphicsContext = null
  _fgFrameListeners = []
  _bgFrameListeners = []
  _attached = false

  constructor (stage, platform, config) {
    super()
    const graphicsContext = createGraphicsContext(stage, platform, config)
    const root = new RootSceneNode(this)
    const { style } = root

    style.backgroundColor = 'black'
    style.top = 0
    style.right = 0
    style.bottom = 0
    style.left = 0
    style.position = 'absolute'

    super.$setup(stage, root, graphicsContext)

    this._stage = stage
    this._root = root
    this._graphicsContext = graphicsContext

    if (typeof config?.title === 'string') {
      this.title = config.title
    }

    if (config.defaultFontSet !== false) {
      loadFontSet(this, config.defaultFontSet)
    }
  }

  on (id, listener) {
    this._emitter.on(id, listener)
  }

  once (id, listener) {
    this._emitter.once(id, listener)
  }

  off (id, listener) {
    this._emitter.off(id, listener)
  }

  get stage () {
    return this._stage
  }

  get root () {
    return this._root
  }

  isAttached () {
    return this._attached
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
    this._graphicsContext.resize(width, height, fullscreen)
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

    super.$frame(tick, lastTick)
  }

  $emit (event) {
    // this._emitter.emitEvent(event)
  }

  $attach () {
    if (this._attached) {
      return
    }

    this._graphicsContext.attach()
    super.$attach()

    this._attached = true
    this._emitter.emitEvent(AttachedEvent(this))
  }

  $detach () {
    if (!this._attached) {
      return
    }

    super.$detach()
    this._graphicsContext.detach()

    this._attached = false
    this._emitter.emitEvent(DetachedEvent(this))
  }

  $destroy () {
    this._emitter.emitEvent(DestroyingEvent(this))

    this._fonts.forEach(font => font.destroy())
    this._fonts = []

    super.$destroy()

    this.$setActiveNode(null)
    // TODO: graphics context destroy() ?

    this._emitter.emitEvent(DestroyedEvent(this))
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

const removeAnimationFrameListener = (requestId, listeners) => {
  const index = listeners.findIndex(value => value[0] === requestId)

  if (index >= 0) {
    // Just clear the listener and preserve the array structure, as the frame may be processing this list right now
    listeners[index] = kEmptyFrameListener
  }
}

const loadFontSet = (self, fontSet) => {
  const isFunction = typeof fontSet === 'function'

  if (fontSet === undefined || fontSet === true || isFunction) {
    const base = dirname(fileURLToPath(import.meta.url))
    let fontDir

    try {
      fontDir = join(base, INTRINSIC_FONT_DIR)
    } catch (e) {
      fontDir = join(base, '..', 'font')
    }

    let fonts = [
      { file: 'Roboto-Regular.ttf', style: 'normal', weight: 'normal' },
      { file: 'Roboto-Bold.ttf', style: 'normal', weight: 'bold' },
      { file: 'Roboto-Italic.ttf', style: 'italic', weight: 'normal' },
      { file: 'Roboto-BoldItalic.ttf', style: 'italic', weight: 'bold' }
    ]

    if (isFunction) {
      fonts = fonts.filter(({ style, weight }) => fontSet(style, weight))
    }

    fontSet = fonts.map(({ file, style, weight }) => `file:${join(fontDir, file)}?family=default&style=${style}&weight=${weight}`)
  }

  if (!Array.isArray(fontSet)) {
    throw Error('fontSet must be an array of font urls or boolean')
  }

  for (const font of fontSet) {
    const fontLink = new LinkSceneNode(self)

    try {
      fontLink.href = font
    } catch (e) {
      continue
    }

    try {
      fontLink.as = 'font'
    } catch (e) {
      continue
    }

    try {
      fontLink.fetch()
    } catch (e) {
      continue
    }

    self._fonts.push(fontLink)
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