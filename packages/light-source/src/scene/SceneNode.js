/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { BlurEvent, FocusEvent } from '../event'
import { addon, Style, StyleClass } from '../addon'

const $bindStyle = Symbol.for('bindStyle')
const $bindStyleClass = Symbol.for('bindStyleClass')

const SceneNodeMixin = (SceneNodeClass) => class extends SceneNodeClass {
  focusable = false
  onKeyUp = null
  onKeyDown = null
  onAxisMotion = null
  onFocus = null
  onBlur = null
  onFocusIn = null
  onFocusOut = null
  waypoint = null
  _hasFocus = false
  _scene = null
  _style = null
  _class = null

  constructor (scene) {
    super(scene)
    this._scene = scene
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
    if (!this._style) {
      this._style = this[$bindStyle](new Style())
    }
    return this._style
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
    if (!styleClass || styleClass instanceof StyleClass) {
      this._class = this.style[$bindStyleClass](styleClass || null)
    }
  }

  /**
   * @returns {Scene} The Scene owner of this node.
   */
  get scene () {
    return this._scene
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

    const event = FocusEvent(this)

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

    const event = BlurEvent(this)

    onBlur?.(event)

    !event.hasStopPropagation() && parent?.$bubble(event, 'onFocusOut')
  }

  /**
   * @returns {boolean} true if this node has focus; otherwise, false
   */
  hasFocus () {
    return this._hasFocus
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
}

export const BoxSceneNode = SceneNodeMixin(addon.BoxSceneNode || class {})
export const ImageSceneNode = SceneNodeMixin(addon.ImageSceneNode || class {})
export const LinkSceneNode = SceneNodeMixin(addon.LinkSceneNode || class {})
export const RootSceneNode = SceneNodeMixin(addon.RootSceneNode || class {})
export const TextSceneNode = SceneNodeMixin(addon.TextSceneNode || class {})
