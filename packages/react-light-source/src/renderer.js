/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ReactContainer } from './ReactContainer'

const $events = Symbol.for('events')
const containers = new Map()

const createContainer = (scene) => {
  if (!scene || !scene[$events]) {
    throw Error('Invalid Scene object.')
  }

  scene[$events].once('destroying', () => {
    const container = containers.get(scene)

    if (container) {
      // render null into the container to release scene graph resources.
      // TODO: does the scheduler have timers active?
      container.render()
    }

    containers.delete(scene)
  })

  containers.set(scene, new ReactContainer(scene))

  return containers.get(scene)
}

export const renderer = (scene) => containers.get(scene) || createContainer(scene)
