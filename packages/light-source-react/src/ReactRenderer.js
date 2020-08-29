/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Element } from './Element'
import { RootElement } from './RootElement'
import { Reconciler } from './Reconciler'

const throwDisconnected = () => {
  throw Error('ReactRenderer has been disconnected and should no longer be in use.')
}

const reconcilers = new Map()

/**
 * @class
 * @ignore
 *
 * Bridge between React and the Light Source Engine scene graph.
 *
 * <p>Internally, the renderer contains a React reconciler and a React container. The reconciler is bound
 * to a Scene. The container is bound to the a SceneNode. All render operations target the container.</p>
 */
export class ReactRenderer {
  sceneListener = null

  constructor (node) {
    let reconciler
    const { scene } = node

    if (reconcilers.has(scene)) {
      reconciler = reconcilers.get(scene)
    } else {
      reconcilers.set(scene, reconciler = Reconciler(scene))
    }

    this._node = node
    this._reconciler = reconciler
    this._container = reconciler.createContainer(new RootElement(node))
  }

  render (component, callback) {
    this._container || throwDisconnected()

    this._reconciler.unbatchedUpdates(
      () => this._reconciler.updateContainer(component, this._container, null, callback))
  }

  findElement (component) {
    this._container || throwDisconnected()

    if (!component || component.nodeType === /* ELEMENT_NODE */1) {
      return component
    }

    const element = this._reconciler.findHostInstance(component)

    if (element instanceof Element) {
      return element
    }

    return null
  }

  findSceneNode (component) {
    this._container || throwDisconnected()

    const element = this.findElement(component)

    return element ? (element.node || null) : null
  }
}
