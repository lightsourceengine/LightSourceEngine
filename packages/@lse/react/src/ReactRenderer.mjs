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

import { Element } from './Element.mjs'
import { RootElement } from './RootElement.mjs'
import { Reconciler } from './Reconciler.mjs'

const throwDisconnected = () => {
  throw Error('ReactRenderer has been disconnected and should no longer be in use.')
}

const reconcilers = new Map()

/**
 * @ignore
 * @class
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
