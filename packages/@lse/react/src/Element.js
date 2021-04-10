/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import { emptyObject } from './emptyObject.js'

const kCommentElementProps = [
  'class',
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
 * Base class for React elements.
 *
 * <p>React elements are specialized components that provide the React reconciler with a way to communicate
 * with a rendering system. React elements appear in jsx with first letter lowercase names, such as &lt;box/&gt;.</p>
 *
 * @memberof module:@lse/react
 * @hideconstructor
 */
class Element {
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
      updateStyle(node, style)
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
const updateStyle = (node, style) => {
  // React re-render with a different state, while preserving the SceneNode graph. The piece meal
  // style settings could accumulate and produce unwanted results. The solution is to clear the piece
  // meal style settings before an update. Note, the class bound to the style remains unchanged.
  node.style.reset()

  // Only support plain objects. StyleClasses, mixins and shorthand are NOT supported. style is only for
  // surgical style updates.
  if (/* isPlainObject: */typeof style === 'object' && style?.constructor === Object) {
    for (const key of Object.keys(style)) {
      // TODO: check key against style property names
      if (key in node.style) {
        node.style[key] = style[key]
      }
    }
  }
}

export { Element }
