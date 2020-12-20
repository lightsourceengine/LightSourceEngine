/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Direction } from '../input/Direction.js'

const $reset = Symbol.for('reset')
const NavigateOpNotSet = 0
const NavigateOpStop = 1
const NavigateOpDefer = 2
const NavigateOpMove = 3
const NavigateOpPass = 4

const throwSelectedNodeCheck = () => {
  throw Error('selected node is required.')
}

class NavigateContext {
  [$reset] (owner, direction) {
    this.owner = owner
    this.direction = direction
    this.op = NavigateOpNotSet
    this.selected = null

    return this
  }

  // Immediately stops the handling of the current event.
  stop () {
    this.op = NavigateOpStop
  }

  // Pass navigation handling to the parent. Used for waypoints that do not handle the direction of the current event.
  pass () {
    this.op = NavigateOpPass
  }

  // Move focus to a new node. The new node must be focusable or have a waypoint.
  move (node) {
    node || throwSelectedNodeCheck()
    this.selected = node
    this.op = NavigateOpMove
  }

  // Defer navigation handling to a future waypoint in the navigate or resolve path.
  defer (node) {
    node || throwSelectedNodeCheck()
    this.selected = node
    this.op = NavigateOpDefer
  }
}

const waypointCapture = (start, direction, event) => {
  if (direction === Direction.NONE) {
    return
  }

  const navigateContext = new NavigateContext()
  let walker = start
  let wasForwarded = false
  let candidate = null

  while (walker) {
    const { waypoint } = walker

    if (waypoint) {
      waypoint.navigate(navigateContext[$reset](walker, direction))

      switch (navigateContext.op) {
        case NavigateOpMove:
          candidate = navigateContext.selected
          event.stopPropagation()
          break
        case NavigateOpPass:
          break
        case NavigateOpStop:
          candidate = null
          event.stopPropagation()
          break
        case NavigateOpDefer:
          candidate = navigateContext.selected
          wasForwarded = true
          break
        default:
          throw Error(`Invalid op: ${navigateContext.op}`)
      }

      if (event.hasStopPropagation()) {
        break
      }
    }

    walker = walker.parent
  }

  if (wasForwarded) {
    event.stopPropagation()
  }

  if (candidate) {
    if (candidate.waypoint) {
      walker = candidate

      while (true) {
        walker = walker.waypoint.resolve({ owner: walker, pending: candidate, direction })

        if (!walker || !(walker.focusable || walker.waypoint)) {
          throw Error('resolve() must return a focus receiver')
        }

        if (walker.focusable) {
          break
        }
      }

      candidate = walker
    } else if (!candidate.focusable) {
      throw Error()
    }

    candidate.focus()
  }
}

/**
 * @ignore
 */
export const eventCapturePhase = (start, direction, event) => {
  if (direction === Direction.NONE) {
    return
  }

  waypointCapture(start, direction, event)
}