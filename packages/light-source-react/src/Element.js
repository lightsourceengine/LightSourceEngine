/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Style } from 'light-source'
import { emptyObject } from './emptyObject'

const throwBadNodeArg = () => { throw Error('Invalid node arg passed to Element constructor.') }
const throwBadPropsArg = () => { throw Error('Invalid props arg passed to Element constructor.') }
const styleTag = Symbol.for('style')

/**
 * @class Element
 * @classdesc
 *
 * Base class for React elements.
 *
 * <p>React elements are specialized components that provide the React reconciler with a way to communicate
 * with a rendering system. React elements appear in jsx with first letter lowercase names, such as &lt;div/&gt;.</p>
 */
export class Element {
  constructor (node, props) {
    node || throwBadNodeArg()
    props || throwBadPropsArg()

    this.node = node
    this.updateProps(emptyObject, props)
  }

  /**
   * Notify element of a property change.
   *
   * @param {Object} oldProps
   * @param {Object} newProps
   */
  updateProps (oldProps, newProps) {
    if (oldProps.style !== newProps.style) {
      const { style } = newProps

      if (!style || style[styleTag]) {
        this.node.style = style
      } else {
        const s = new Style()

        // TODO: move this into a factory method
        for (const prop in style) {
          if (prop in Style.prototype) {
            s[prop] = style[prop]
          }
        }

        this.node.style = s
      }
    }

    this.node.focusable = !!newProps.focusable

    if (oldProps.hidden !== newProps.hidden) {
      this.node.hidden = (newProps.hidden === undefined) || !!newProps.hidden
    }

    if (oldProps.onFocus !== newProps.onFocus) {
      this.node.onFocus = newProps.onFocus
    }

    if (oldProps.onBlur !== newProps.onBlur) {
      this.node.onBlur = newProps.onBlur
    }

    if (oldProps.onKeyUp !== newProps.onKeyUp) {
      this.node.onKeyUp = newProps.onKeyUp
    }

    if (oldProps.onKeyDown !== newProps.onKeyDown) {
      this.node.onKeyDown = newProps.onKeyDown
    }

    if (oldProps.onAxisMotion !== newProps.onAxisMotion) {
      this.node.onAxisMotion = newProps.onAxisMotion
    }

    if (oldProps.onDeviceButtonUp !== newProps.onDeviceButtonUp) {
      this.node.onDeviceButtonUp = newProps.onDeviceButtonUp
    }

    if (oldProps.onDeviceButtonDown !== newProps.onDeviceButtonDown) {
      this.node.onDeviceButtonDown = newProps.onDeviceButtonDown
    }

    if (oldProps.onDeviceAxisMotion !== newProps.onDeviceAxisMotion) {
      this.node.onDeviceAxisMotion = newProps.onDeviceAxisMotion
    }

    this.props = newProps
  }

  /**
   * Append a child element.
   *
   * @param {Element} child
   */
  appendChild (child) {
    this.node.appendChild(child.node)
  }

  /**
   * Insert a child element before another child element.
   *
   * @param {Element} child
   * @param {Element} before
   */
  insertBefore (child, before) {
    this.node.insertBefore(child.node, before.node)
  }

  /**
   * Remove a child element.
   *
   * @param {Element} child
   */
  removeChild (child) {
    child.node.destroy()
  }
}
