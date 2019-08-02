/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Reconciler } from './Reconciler'
import { RootElement } from './RootElement'

const $events = Symbol.for('events')

export class ReactContainer {
  constructor (scene) {
    if (!scene || !scene[$events]) {
      throw Error('Invalid Scene object.')
    }

    scene[$events].once('destroying', () => {
      this.render()
    })

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

    if (element) {
      return element.node
    }

    return null
  }
}
