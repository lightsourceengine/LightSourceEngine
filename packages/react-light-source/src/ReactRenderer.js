/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element'
import { RootElement } from './RootElement'

const $reconciler = Symbol.for('reconciler')
const $container = Symbol.for('container')
const $disconnecting = Symbol.for('disconnecting')
const $node = Symbol.for('node')

const emptyDisconnecting = (node) => {}

const throwDisconnected = () => {
  throw Error('ReactRenderer has been disconnected and should no longer be in use.')
}

export class ReactRenderer {
  /**
   * @class ReactRenderer
   * @classdesc
   *
   * Bridge between React and the Light Source Engine scene graph.
   *
   * <p>Internally, the renderer contains a React reconciler and a React container. The reconciler is bound
   * to a Scene. The container is bound to the a SceneNode. All render operations target the container.</p>
   *
   * @param {Reconciler} reconciler React reconciler associated with a Scene
   * @param {SceneNode} node SceneNode to render in to
   */
  constructor (reconciler, node) {
    this[$reconciler] = reconciler
    this[$node] = node
    this[$container] = reconciler.createContainer(new RootElement(node))
    this[$disconnecting] = emptyDisconnecting
  }

  /**
   * Render a React component.
   *
   * <p>This method is functionally equivalent to ReactDOM.render().</p>
   *
   * @param {Object} component The React component to create or update. If null, any previously rendered components will
   * be unmounted.
   * @param {function} [callback] Executed after the component has been rendered, updated or unmounted.
   * @throws {Error} When the method is called on a disconnected renderer
   */
  render (component, callback) {
    this[$container] || throwDisconnected()

    this[$reconciler].unbatchedUpdates(
      () => this[$reconciler].updateContainer(component, this[$container], null, callback))
  }

  /**
   * Disconnect the SceneNode from React.
   *
   * <p>After calling disconnect(), this object is effectively destroyed and subsequent calls will result in
   * Error exceptions. If the intent is to just unmount components, call render(null).</p>
   *
   * @param {function} [callback] Executed after the SceneNode has been disconnected from React.
   * @returns {boolean} if true, the disconnect operation has been scheduled. if false, the renderer has already
   * been disconnected
   */
  disconnect (callback) {
    if (!this[$container]) {
      return false
    }

    this.render(null, () => {
      this[$disconnecting] && this[$disconnecting](this[$node])
      this[$reconciler] = this[$node] = this[$container] = this[$disconnecting] = null
      callback && callback()
    })

    return true
  }

  /**
   * Find an Element given a React component instance.
   *
   * <p>This method is similar to ReactDOM.findDOMNode().</p>
   *
   * <p>Note, this method is here for completeness, but React refs should be used to access SceneNodes from
   * React components.</p>
   *
   * @param component React component instance
   * @returns {(Element|null)}
   * @throws {Error} When the method is called on a disconnected renderer
   */
  findElement (component) {
    this[$container] || throwDisconnected()

    if (!component || component.nodeType === /* ELEMENT_NODE */1) {
      return component
    }

    const element = this[$reconciler].findHostInstance(component)

    if (element instanceof Element) {
      return element
    }

    return null
  }

  /**
   * Find a SceneNode given a React component instance.
   *
   * <p>This method is similar to ReactDOM.findDOMNode().</p>
   *
   * <p>Note, this method is here for completeness, but React refs should be used to access SceneNodes from
   * React components.</p>
   *
   * @param component React component instance
   * @returns {(Element|null)}
   * @throws {Error} When the method is called on a disconnected renderer
   */
  findSceneNode (component) {
    this[$container] || throwDisconnected()

    const element = this.findElement(component)

    return element ? (element.node || null) : null
  }
}
