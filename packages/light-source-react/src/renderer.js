/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ReactRenderer } from './ReactRenderer'

const renderersByContainer = new Map()
const nullPrototype = { constructor: { name: 'null' } }

/**
 * Render a React component.
 *
 * <p>This method is functionally equivalent to ReactDOM.render().</p>
 *
 * @param {Scene|SceneNode} container The SceneNode that will be the parent of element. If container is a Scene,
 * scene.root is used as the container.
 * @param {Object} element The React component/element to create or update. If null, any previously rendered
 * components will be unmounted from the parent container.
 * @param {function} [callback] Executed after the component has been rendered, updated or unmounted.
 * @throws {Error} when the container or callback is an invalid type
 */
export const render = (container, element, callback) => {
  if (callback && typeof callback !== 'function') {
    throw Error('callback must be a function, undefined or null')
  }

  if (!callback) {
    callback = null
  }

  const node = resolveContainer(container)

  if (!element) {
    if (renderersByContainer.has(node)) {
      const renderer = renderersByContainer.get(node)

      renderersByContainer.delete(node)
      renderer.render(null, callback)
    } else {
      queueMicrotask(callback)
    }
  } else {
    let renderer = renderersByContainer.get(node)

    if (!renderer) {
      renderersByContainer.set(node, renderer = new ReactRenderer(node))
    }

    renderer.render(element, callback)
  }
}

/**
 * Find an Element given a React component instance.
 *
 * <p>This method is similar to ReactDOM.findDOMNode().</p>
 *
 * <p>Note, this method is here for completeness, but React refs should be used to access SceneNodes from
 * React components.</p>
 *
 * @param container {Scene|SceneNode} The parent node to search on.
 * @param component React class-based component instance
 * @returns {(Element|null)}
 * @throws {Error} When the method is called on a disconnected renderer
 */
export const findElement = (container, component) => {
  const node = resolveContainer(container)

  if (renderersByContainer.has(node)) {
    return renderersByContainer.get(node).findElement(component)
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
 * @param container {Scene|SceneNode} The parent node to search on.
 * @param component React component instance
 * @returns {(Element|null)}
 * @throws {Error} When the method is called on a disconnected renderer
 */
export const findSceneNode = (container, component) => {
  const node = resolveContainer(container)

  if (renderersByContainer.has(node)) {
    return renderersByContainer.get(node).findSceneNode(component)
  }

  return null
}

export const getActiveContainers = () => Array.from(renderersByContainer.keys())

const getClassName = obj => (typeof obj === 'object' ? Object.getPrototypeOf(obj) : nullPrototype).constructor.name

const resolveContainer = (container) => {
  if (getClassName(container) === 'Scene') {
    return container.root
  } else if (getClassName(container?.scene) === 'Scene') {
    return container
  } else {
    throw Error('container must be of type Scene or SceneNode')
  }
}
