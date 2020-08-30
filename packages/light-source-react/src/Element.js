/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { emptyObject } from './emptyObject'
import { createStyle } from 'light-source'

const kCommentElementProps = [
  'focusable',
  'hidden',
  'onFocus',
  'onFocusIn',
  'onFocusOut',
  'onBlur',
  'onKeyUp',
  'onKeyDown',
  'onAxisMotion'
]

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
    const { node } = this
    const { style } = newProps

    if (oldProps.style !== style) {
      // createStyle returns style if already a Style instance
      node.style = createStyle(style)
    }

    for (const name of kCommentElementProps) {
      if (oldProps[name] !== newProps[name]) {
        node[name] = newProps[name]
      }
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

const throwBadNodeArg = () => { throw Error('Invalid node arg passed to Element constructor.') }
const throwBadPropsArg = () => { throw Error('Invalid props arg passed to Element constructor.') }
