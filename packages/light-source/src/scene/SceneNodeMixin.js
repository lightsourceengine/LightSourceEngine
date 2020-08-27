/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { BlurEvent, FocusEvent } from '../event'

/**
 * Mixin that adds javascript specific behavior to native SceneNode classes.
 *
 * @param SceneNodeClass SceneNode native class to mix
 * @returns {*} SceneNode class
 * @constructor
 */
export const SceneNodeMixin = (SceneNodeClass) => class extends SceneNodeClass {
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
