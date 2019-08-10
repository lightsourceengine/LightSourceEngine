/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Direction } from './Direction'

const $navigation = Symbol.for('navigation')
const $focalPath = Symbol.for('focalPath')
const $focalPathIndex = Symbol.for('focalPathIndex')

const horizontalTag = 'horizontal'
const verticalTag = 'vertical'

const HORIZONTAL = 1
const VERTICAL = 2
const OFFSET = []

OFFSET[0] = []

OFFSET[HORIZONTAL] = []
OFFSET[HORIZONTAL][Direction.LEFT] = -1
OFFSET[HORIZONTAL][Direction.RIGHT] = 1

OFFSET[VERTICAL] = []
OFFSET[VERTICAL][Direction.UP] = -1
OFFSET[VERTICAL][Direction.DOWN] = 1

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

    this[$focalPath] = null
    this[$focalPathIndex] = -1
  }

  navigate (owner, navigate) {
    this._createFocalPath(owner)

    const offset = OFFSET[this[$navigation]][navigate.direction]

    if (!offset) {
      navigate.pass()
      return
    }

    const nextFocalPathIndex = this[$focalPathIndex] + offset
    const focalPath = this[$focalPath]

    if (nextFocalPathIndex >= 0 && nextFocalPathIndex < focalPath.length) {
      navigate.done(focalPath[this[$focalPathIndex] = nextFocalPathIndex])
    } else {
      navigate.continue(focalPath[this[$focalPathIndex]])
    }
  }

  resolve (owner, navigate) {
    this._createFocalPath(owner)

    let focalPathIndex = this[$focalPathIndex]
    const navigation = this[$navigation]
    const focalPath = this[$focalPath]
    const { direction, pending } = navigate

    switch (direction) {
      case Direction.NONE:
        this._syncChildFocus(navigate)
        break
      case Direction.DOWN:
        if (pending && navigation === VERTICAL && !isDescendent(pending, owner)) {
          focalPathIndex = 0
        }
        break
      case Direction.UP:
        if (pending && navigation === VERTICAL && !isDescendent(pending, owner)) {
          focalPathIndex = focalPath.length - 1
        }
        break
      case Direction.RIGHT:
        if (pending && navigation === HORIZONTAL && !isDescendent(pending, owner)) {
          focalPathIndex = 0
        }
        break
      case Direction.LEFT:
        if (pending && navigation === HORIZONTAL && !isDescendent(pending, owner)) {
          focalPathIndex = focalPath.length - 1
        }
        break
      default:
        throw Error('Unknown direction: ' + direction)
    }

    return navigate.done(focalPath[(this[$focalPathIndex] = focalPathIndex)])
  }

  _createFocalPath (app) {
    const path = []
    let result

    for (const child of app.children) {
      (result = findFocusable(child)) && path.push(result)
    }

    if (path.length === 0) {
      throw Error('Waypoint expects to be associated with a node with focusable children.')
    }

    this[$focalPath] = path
  }

  _syncChildFocus (navigate) {
    const focalPath = this[$focalPath]
    const { pending } = navigate

    if (focalPath[this[$focalPathIndex]] !== pending) {
      let i = focalPath.length - 1

      while (i--) {
        if (focalPath[i] === pending) {
          this.focalPathIndex = i
          return navigate.done(focalPath[i])
        }
      }
    }

    return navigate.pass()
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
