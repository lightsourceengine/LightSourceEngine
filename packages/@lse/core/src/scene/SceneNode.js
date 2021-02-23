/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createBlurEvent, createFocusEvent } from '../event/index.js'
import {
  CBoxSceneNode,
  CImageSceneNode,
  CRootSceneNode,
  CTextSceneNode,
  setStyleParent
} from '../addon/index.js'
import { Style } from '../style/Style.js'
import { emptyArray } from '../util/index.js'

class SceneNode {
  focusable = false
  onKeyUp = null
  onKeyDown = null
  onFocus = null
  onBlur = null
  onFocusIn = null
  onFocusOut = null
  _hasFocus = false
  _parent = null
  _scene = null
  _style = null
  _class = null
  _children = emptyArray
  _native

  constructor (scene, native) {
    this._native = native
    this._scene = scene

    if (!this.isLeaf()) {
      this._children = []
    }
  }

  /**
   * Style interface for this node.
   *
   * The style object can be used to set and get the style properties of this node. Get returns the current state
   * of the style that will be used for rendering. The set is used to manually set individual properties, overriding
   * any properties set by the style class.
   *
   * @returns {Style} Instance to the style object of this node.
   */
  get style () {
    return this._style || (this._style = nativeBindStyle(this._native))
  }

  /**
   * Set and get the style class of this node.
   *
   * The style class must be a StyleClass instance or null. The style class sets the base properties of the
   * style object of this node. When properties are manually set on the style object, those properties take
   * precedence over the style class properties.
   *
   * @property class {StyleClass}
   */
  get class () {
    return this._class
  }

  set class (styleClass) {
    setStyleParent(this.style, styleClass)
    this._class = styleClass ?? null
  }

  /**
   * @returns {Scene} The Scene owner of this node.
   */
  get scene () {
    return this._scene
  }

  get parent () {
    return this._parent
  }

  get children () {
    return this._children
  }

  /**
   * Sets focus to this node.
   *
   * The scene's activeNode will be updated on this call.
   *
   * Focus events will be dispatched on this call.
   *
   * If another node already has focus, that node will be unfocused and the appropriate events will be dispatched.
   *
   * If this node is already focused or the node is not focusable, this call is a no-op.
   */
  focus () {
    if (!this.focusable || this._hasFocus) {
      return
    }

    const { scene, parent, onFocus } = this
    const { activeNode } = scene

    activeNode !== this && activeNode?.blur()

    scene.$setActiveNode(this)
    this._hasFocus = true

    const event = createFocusEvent(this)

    onFocus?.(event)

    !event.hasStopPropagation() && parent?.$bubble(event, 'onFocusIn')
  }

  /**
   * Clears the focus of this node.
   *
   * If this node does not have focus, this call is a no-op.
   *
   * If this not loses focus, the appropriate events will be dispatched on this call.
   */
  blur () {
    if (!this._hasFocus) {
      return
    }

    const { scene, parent, onBlur } = this

    this._hasFocus = false

    if (scene.activeNode === this) {
      scene.$setActiveNode(null)
    }

    const event = createBlurEvent(this)

    onBlur?.(event)

    !event.hasStopPropagation() && parent?.$bubble(event, 'onFocusOut')
  }

  /**
   * @returns {boolean} true if this node has focus; otherwise, false
   */
  hasFocus () {
    return this._hasFocus
  }

  appendChild (node) {
    this.isLeaf() && throwAddChildError()

    nativeAppendChild(this._native, node?._native)
    this._children.push(node)
    node._parent = this
  }

  insertBefore (node, before) {
    this.isLeaf() && throwAddChildError()

    const { _children, _native } = this
    const index = _children.findIndex(value => value === before)

    index >= 0 || throwBeforeArgError()

    _native.insertBefore(node?._native, before?._native)
    _children.splice(index, 0, node)
    node._parent = this
  }

  removeChild (node) {
    const { _children, _native } = this
    const index = _children.findIndex(value => value === node)

    if (index !== -1) {
      nativeRemoveChild(_native, node._native)
      _children[index]._parent = null
      _children.splice(index, 1)
    }
  }

  isLeaf () {
    return false
  }

  destroy () {
    this._native && this._destroy()
  }

  /**
   * @ignore
   */
  $bubble (event, prop) {
    let walker = this

    while (walker) {
      event.currentTarget = walker
      walker[prop]?.(event)

      if (event.hasStopPropagation()) {
        break
      }

      walker = walker.parent
    }
  }

  /**
   * @ignore
   */
  get $native () {
    return this._native
  }

  /**
   * @ignore
   */
  _destroy () {
    const { _parent, _children, _native } = this

    _parent?.removeChild(this)

    while (_children.length) {
      _children[_children.length - 1]._destroy()
    }

    nativeDestroy(_native)

    this._children = emptyArray
    this._scene = this._style = this._class = this._native = null
    this.onKeyUp = this.onKeyDown = this.onFocus = this.onBlur = this.onFocusIn = this.onFocusOut = null
  }
}

export class BoxSceneNode extends SceneNode {
  waypoint = null

  constructor (scene) {
    super(scene, new CBoxSceneNode(scene.$native))
  }
}

const $onLoad = Symbol('onLoad')
const $onError = Symbol('onError')
const $src = Symbol('src')
const kEmptySource = Object.freeze({ uri: '' })

export class ImageSceneNode extends SceneNode {
  [$onLoad] = null;
  [$onError] = null;
  [$src] = null;

  constructor (scene) {
    super(scene, new CImageSceneNode(scene.$native))
  }

  get src () {
    return this[$src]
  }

  set src (value) {
    let source

    if (value) {
      if (typeof value === 'string') {
        source = { uri: value }
      } else if (value.uri && typeof value.uri === 'string') {
        source = value
      }
    }

    this._native.setSource(source)
    this[$src] = source ?? kEmptySource
  }

  get onLoad () {
    return this[$onLoad]
  }

  set onLoad (value) {
    this._native.cb || setImageStatusCallback(this)
    this[$onLoad] = typeof value === 'function' ? value : null
  }

  get onError () {
    return this[$onError]
  }

  set onError (value) {
    this._native.cb || setImageStatusCallback(this)
    this[$onError] = typeof value === 'function' ? value : null
  }

  isLeaf () {
    return true
  }

  _destroy () {
    this._native.cb && clearImageStatusCallback(this)
    this[$onLoad] = this[$onError] = this[$src] = null
    super._destroy()
  }
}

export class RootSceneNode extends SceneNode {
  constructor (scene) {
    super(scene, new CRootSceneNode(scene.$native))
  }
}

export class TextSceneNode extends SceneNode {
  constructor (scene) {
    super(scene, new CTextSceneNode(scene.$native))
  }

  get text () {
    return this._native.text
  }

  set text (value) {
    this._native.text = value
  }

  isLeaf () {
    return true
  }
}

const throwAddChildError = () => {
  throw Error('leaf nodes cannot have children')
}

const throwBeforeArgError = () => {
  throw Error('before is not a child')
}

// if native calls are present in a function, the function will not be eligible for v8 jit. isolate the calls into
// separate function so the callers can be eligible for optimization. (Just doing this for frequently called functions)
const nativeBindStyle = (native) => native.bindStyle(new Style())
const nativeAppendChild = (native, child) => native.appendChild(child)
const nativeRemoveChild = (native, child) => native.removeChild(child)
const nativeDestroy = (native) => native.destroy()

const setImageStatusCallback = (node) => {
  const self = node
  self._native.setCallback((img, errorCode) => {
    if (errorCode) {
      self[$onError]?.(img, errorCode)
    } else {
      self[$onLoad]?.(img)
    }
  })
}

const clearImageStatusCallback = (node) => {
  node._native.setCallback(null)
}
