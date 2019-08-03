/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Reconciler } from './Reconciler'
import { RootElement } from './RootElement'

export class ReactContainer {
  constructor (scene) {
    this._reconciler = Reconciler(scene)
    this._container = this._reconciler.createContainer(new RootElement(scene.root))
  }

  render (component, callback) {
    return this._reconciler.updateContainer(component, this._container, null, callback)
  }

  findElement (component) {
    if (!component || component.nodeType === /* ELEMENT_NODE */1) {
      return component
    }

    return this._reconciler.findHostInstance(component)
  }

  findSceneNode (component) {
    const element = this.findElement(component)

    return element.node || null
  }
}
