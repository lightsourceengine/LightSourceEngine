/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { SceneBase, logger } from '../addon/index.js'
import { BoxSceneNode, ImageSceneNode, TextSceneNode, LinkSceneNode, RootSceneNode } from './SceneNode.js'
import { createAttachedEvent, createDestroyedEvent, createDestroyingEvent, createDetachedEvent } from '../event/index.js'
import { EventName } from '../event/EventName.js'
import { fileURLToPath } from 'url'
import { join, dirname, isAbsolute } from 'path'
import { readFileSync, existsSync } from 'fs'
import { stringify } from 'querystring'
import { EventTarget } from '../event/EventTarget.js'

const kEmptyFrameListener = Object.freeze([0, null])
let sFrameRequestId = 0

export class Scene extends EventTarget {
  _activeNode = null

  _stage = null
  _root = null
  _fonts = []
  _context = null
  _fgFrameListeners = []
  _bgFrameListeners = []
  _attached = false
  $native = new SceneBase()

  constructor (stage, config) {
    super([
      EventName.onAttached,
      EventName.onDetached,
      EventName.onDestroying,
      EventName.onDestroyed
    ])

    const context = stage.system.$createGraphicsContext(config)
    const root = new RootSceneNode(this)
    const { style } = root

    style.backgroundColor = 'black'
    style.top = 0
    style.right = 0
    style.bottom = 0
    style.left = 0
    style.position = 'absolute'

    this.$native.$setup(stage.$native, root, context.$native)

    this._stage = stage
    this._root = root
    this._context = context

    if (typeof config?.title === 'string') {
      this.title = config.title
    }

    loadFonts(this)
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

    this.$native.$frame(tick, lastTick)
  }

  $attach () {
    if (this._attached) {
      return
    }

    this.$native.$attach()

    this._attached = true
    this.dispatchEvent(createAttachedEvent(this))
  }

  $detach () {
    if (!this._attached) {
      return
    }

    this.$native.$detach()

    this._attached = false
    this.dispatchEvent(createDetachedEvent(this))
  }

  $destroy () {
    this.dispatchEvent(createDestroyingEvent(this))

    this._fonts.forEach(font => font.destroy())
    this._fonts = []

    this._context = null
    this.$native.$destroy()

    this.$setActiveNode(null)
    // TODO: graphics context destroy() ?

    this.dispatchEvent(createDestroyedEvent(this))
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

const loadFonts = (self) => {
  const filename = 'font.manifest'
  const { LSE_FONT_PATH, LSE_ENV } = process.env
  let fontManifestFile

  if (LSE_FONT_PATH) {
    fontManifestFile = join(LSE_FONT_PATH, filename)
  } else if (LSE_ENV !== 'lse-node') {
    // for non-lse-node environments (mono-repo and npm install), search for font.manifest relative to
    // this file. lse-node runs just use the environment variable.
    const dir = dirname(fileURLToPath(import.meta.url))

    let p = join(dir, '..', 'font', filename)

    if (existsSync(p)) {
      fontManifestFile = p
    } else {
      p = join(dir, 'font', filename)

      if (existsSync(p)) {
        fontManifestFile = p
      }
    }
  }

  let fontManifest

  try {
    fontManifest = JSON.parse(readFileSync(fontManifestFile, 'utf8'))
  } catch (e) {
    logger.warn(`Error loading '${fontManifestFile}': ${e.message}`)
    return
  }

  if (!Array.isArray(fontManifest)) {
    logger.warn(`Expected array from '${fontManifestFile}'`)
    return
  }

  const directory = dirname(fontManifestFile)

  for (let { file, family, style, weight } of fontManifest) {
    if (typeof file !== 'string') {
      continue
    }

    if (!isAbsolute(file)) {
      file = join(directory, file)
    }

    family = family?.toString() ?? undefined
    style = style?.toString() ?? undefined
    weight = weight?.toString() ?? undefined

    const url = 'file:' + file + '?' + stringify({ family, style, weight })
    const fontLink = new LinkSceneNode(self)

    self._fonts.push(fontLink)

    try {
      fontLink.href = url
      fontLink.as = 'font'
      fontLink.fetch()
    } catch (e) {
      logger.warn(`Failed to preload font: ${url}`)
    }
  }
}
