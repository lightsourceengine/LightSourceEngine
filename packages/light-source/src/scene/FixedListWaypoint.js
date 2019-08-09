/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Direction } from './Direction'

const HORIZONTAL = 1
const VERTICAL = 2

const OFFSET = []

OFFSET[HORIZONTAL] = []
OFFSET[HORIZONTAL][Direction.LEFT] = -1
OFFSET[HORIZONTAL][Direction.RIGHT] = 1

OFFSET[VERTICAL] = []
OFFSET[VERTICAL][Direction.UP] = -1
OFFSET[VERTICAL][Direction.DOWN] = 1

export class FixedListWaypoint {
  constructor (navigation) {
    this.navigation = navigation
  }

  navigate (owner, navigate) {
    this._createFocalPath(owner)

    const offset = OFFSET[this.navigation][navigate.direction]

    if (!offset) {
      navigate.pass()
      return
    }

    const nextFocalPathIndex = this.focalPathIndex + offset

    if (nextFocalPathIndex >= 0 && nextFocalPathIndex < this._focalPath.length) {
      navigate.done(this._focalPath[this.focalPathIndex = nextFocalPathIndex])
    } else {
      navigate.continue(this._focalPath[this.focalPathIndex])
    }
  }

  resolve (owner, navigate) {
    this._createFocalPath(owner)

    let { navigation, _focalPath, focalPathIndex } = this
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
          focalPathIndex = _focalPath.length - 1
        }
        break
      case Direction.RIGHT:
        if (pending && navigation === HORIZONTAL && !isDescendent(pending, owner)) {
          focalPathIndex = 0
        }
        break
      case Direction.LEFT:
        if (pending && navigation === HORIZONTAL && !isDescendent(pending, owner)) {
          focalPathIndex = _focalPath.length - 1
        }
        break
      default:
        throw Error('Unknown direction: ' + direction)
    }

    return navigate.done(_focalPath[(this.focalPathIndex = focalPathIndex)])
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

    this._focalPath = path
  }

  _syncChildFocus (navigate) {
    const { _focalPath, focalPathIndex } = this
    const { pending } = navigate

    if (_focalPath[focalPathIndex] !== pending) {
      let i = _focalPath.length - 1

      while (i--) {
        if (_focalPath[i] === pending) {
          this.focalPathIndex = i
          return navigate.done(_focalPath[i])
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
