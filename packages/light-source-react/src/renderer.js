/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ReactRenderer } from './ReactRenderer'
import { Reconciler } from './Reconciler'

const $events = Symbol.for('events')
const $disconnecting = Symbol.for('disconnecting')

const reconcilers = new Map()
const containers = new Map()

const createContainer = (reconciler, node) => {
  const container = containers.set(node, new ReactRenderer(reconciler, node)).get(node)

  container[$disconnecting] = (node) => {
    containers.delete(node)

    for (const key of containers.keys()) {
      if (node.scene === key.scene) {
        return
      }
    }

    reconcilers.delete(node.scene)
  }

  return container
}

const createReconciler = (scene) => {
  scene[$events].once('destroying', () => {
    reconcilers.delete(scene)

    for (const node of containers.keys()) {
      if (node.scene === scene) {
        containers.delete(node)
      }
    }

    if (global.MessageChannel) {
      global.MessageChannel.close()
    }
  })

  return reconcilers.set(scene, new Reconciler(scene)).get(scene)
}

const getClassName = obj => {
  const proto = obj ? Object.getPrototypeOf(obj) : null

  return proto ? proto.constructor.name : ''
}

/**
 * @method renderer
 *
 * @description
 *
 * Gets a ReactRenderer associated with the given SceneNode.
 *
 * <p>If a renderer has already been created for the given SceneNode, it will be returned. Otherwise, a new
 * renderer will be created and stored internally. The lifecycle of renderers created through this function
 * are bound to the lifecycle of the SceneNode's Scene. To clean up a renderer before it's Scene is destroyed,
 * call ReactRenderer#disconnect().</p>
 *
 * <p>When the argument is a Scene, the renderer will be associated with Scene.root, the common case.</p>
 *
 * @example
 * import { stage } from 'light-source'
 * import { renderer } from 'react-light-source'
 * import React from 'react'
 *
 * const scene = stage.createScene()
 *
 * renderer(scene).render(<MyComponent/>)
 *
 * @param {(Scene|SceneNode)} sceneOrSceneNode The SceneNode to be associated with the ReactRenderer
 * @returns {ReactRenderer}
 * @throws {Error} When sceneOrSceneNode is not an instance of Scene or SceneNode
 */
export const renderer = (sceneOrSceneNode) => {
  const className = getClassName(sceneOrSceneNode)

  if (className === 'Scene') {
    const { root } = sceneOrSceneNode

    if (!root) {
      throw Error('Scene argument does not contain a root SceneNode')
    }

    if (containers.has(root)) {
      return containers.get(root)
    } else {
      return createContainer(reconcilers.get(sceneOrSceneNode) || createReconciler(sceneOrSceneNode), root)
    }
  } else if (className.endsWith('SceneNode')) {
    if (containers.has(sceneOrSceneNode)) {
      return containers.get(sceneOrSceneNode)
    } else {
      const { scene } = sceneOrSceneNode

      if (getClassName(scene) !== 'Scene') {
        throw Error('SceneNode argument does not appear to be associated with a Scene')
      }

      return createContainer(reconcilers.get(scene) || createReconciler(scene), sceneOrSceneNode)
    }
  } else {
    throw Error(`renderer() expects a Scene or SceneNode as an argument. Got: ${sceneOrSceneNode}`)
  }
}

export { containers as _testOnlyContainers, reconcilers as _testOnlyReconcilers }
