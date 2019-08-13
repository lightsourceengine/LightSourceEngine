/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Direction } from './Direction'
import { StandardKey } from '../input/StandardKey'
import { $setActiveNode } from '../util/InternalSymbols'

const standardKeyToDirection = new Map([
  [StandardKey.UP, Direction.UP],
  [StandardKey.RIGHT, Direction.RIGHT],
  [StandardKey.DOWN, Direction.DOWN],
  [StandardKey.LEFT, Direction.LEFT]
])

export const navigationManager = (scene, focus, event) => {
  const { key } = event

  if (!standardKeyToDirection.has(key)) {
    return
  }

  const direction = standardKeyToDirection.get(key)
  let walker = scene.activeNode
  let candidate

  while (walker) {
    const { waypoint } = walker

    if (waypoint) {
      const [node, moved] = waypoint.navigate(walker, direction)

      if (node) {
        if (moved) {
          candidate = node
          break
        } else if (!candidate) {
          candidate = node
        }
      }
    }

    walker = walker.parent
  }

  if (candidate) {
    event.stop()

    if (candidate.waypoint) {
      walker = candidate

      while (true) {
        walker = walker.waypoint.resolve(walker, candidate, direction)

        if (!walker) {
          throw Error('resolve() must return a focus candidate.')
        }

        if (walker.focusable) {
          break
        }

        // TODO: has waypoint
      }

      candidate = walker
    }

    if (!candidate || !candidate.focusable) {
      throw Error()
    }

    scene[$setActiveNode](candidate)
  }
}
