/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Direction } from '../input/Direction'
import { $hasFocus } from '../util/InternalSymbols'

const $navigation = Symbol.for('navigation')
const $focalPathIndex = Symbol.for('focalPathIndex')
const pass = Object.freeze([null, false])

const horizontalTag = 'horizontal'
const verticalTag = 'vertical'

const { UP, DOWN, LEFT, RIGHT } = Direction

const HORIZONTAL = 1
const VERTICAL = 2
const OFFSET = []

OFFSET[0] = []

OFFSET[HORIZONTAL] = []
OFFSET[HORIZONTAL][LEFT] = -1
OFFSET[HORIZONTAL][RIGHT] = 1

OFFSET[VERTICAL] = []
OFFSET[VERTICAL][UP] = -1
OFFSET[VERTICAL][DOWN] = 1

export class FixedListWaypoint {
  constructor (tag) {
    if (tag === horizontalTag) {
      this[$navigation] = HORIZONTAL
      this.tag = horizontalTag
    } else if (tag === verticalTag) {
      this[$navigation] = VERTICAL
      this.tag = verticalTag
    } else {
      throw Error(`FixedListWaypoint tag should be ${horizontalTag} or ${verticalTag}. Got ${tag}`)
    }

    this[$focalPathIndex] = -1
  }

  navigate (owner, direction) {
    const offset = OFFSET[this[$navigation]][direction]

    if (!offset) {
      return pass
    }

    const nextFocalPathIndex = this[$focalPathIndex] + offset
    const focalPath = createFocalPath(owner)

    // TODO: index out of range?

    let moved = false

    if (nextFocalPathIndex >= 0 && nextFocalPathIndex < focalPath.length) {
      this[$focalPathIndex] = nextFocalPathIndex
      moved = true
    }

    return [focalPath[this[$focalPathIndex]], moved]
  }

  resolve (owner, pending, direction) {
    let focalPathIndex = this[$focalPathIndex]
    const navigation = this[$navigation]
    const focalPath = createFocalPath(owner)

    switch (direction) {
      case DOWN:
        if (pending && navigation === VERTICAL && !isDescendent(pending, owner)) {
          focalPathIndex = 0
        }
        break
      case UP:
        if (pending && navigation === VERTICAL && !isDescendent(pending, owner)) {
          focalPathIndex = focalPath.length - 1
        }
        break
      case RIGHT:
        if (pending && navigation === HORIZONTAL && !isDescendent(pending, owner)) {
          focalPathIndex = 0
        }
        break
      case LEFT:
        if (pending && navigation === HORIZONTAL && !isDescendent(pending, owner)) {
          focalPathIndex = focalPath.length - 1
        }
        break
      default:
        throw Error(`Unknown direction: ${direction}`)
    }

    // TODO: out of range

    if (focalPathIndex === -1) {
      focalPathIndex = 0
    }

    return focalPath[(this[$focalPathIndex] = focalPathIndex)]
  }

  sync (owner) {
    let i = 0

    for (const node of createFocalPath(owner)) {
      if (node[$hasFocus]) {
        this[$focalPathIndex] = i
        return
      }
      i++
    }

    // this[$focalPathIndex] = -1
  }
}

const findFocusable = (node) => {
  if (node.waypoint || node.focusable) {
    return node
  }

  let result

  for (const child of node.children) {
    if ((result = findFocusable(child))) {
      return result
    }
  }

  return null
}

const createFocalPath = (node) => {
  const path = []
  let result

  for (const child of node.children) {
    (result = findFocusable(child)) && path.push(result)
  }

  if (path.length === 0) {
    throw Error('Waypoint expects to be associated with a node with focusable children.')
  }

  return path
}

const isDescendent = (a, b) => {
  let walker = a

  while (walker != null) {
    if (b === walker.parent) {
      return true
    }

    walker = walker.parent
  }

  return false
}
