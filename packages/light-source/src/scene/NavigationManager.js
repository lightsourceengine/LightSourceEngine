/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Direction } from './Direction'

class NavigationContext {
  static DONE = 1

  static CONTINUE = 3

  static ABORT = 4

  static SYNC_CHILD = Direction.NONE

  constructor (direction) {
    this.direction = direction
    this.pending = null
    this.command = null
    this.next = null
  }

  continue (view) {
    this.command = NavigationContext.CONTINUE
    this.pending = this.pending || view
  }

  pass () {
    this.command = NavigationContext.CONTINUE
  }

  done (view) {
    this.command = NavigationContext.DONE
    this.next = view
  }

  abort () {
    this.command = NavigationContext.ABORT
  }
}

const syncChildFocus = (focused) => {
  const navigate = new NavigationContext(NavigationContext.SYNC_CHILD)
  let walker = focused.parent
  let focusedChild = focused

  // Focus has been changed outside of directional navigation. Go through all parent waypoints to ensure
  // they are in sync with the focus change.
  while (walker) {
    const { waypoint, parent } = walker

    if (waypoint) {
      navigate.pending = focusedChild
      navigate.next = null

      waypoint.resolve(walker, navigate)
      focusedChild = walker
    }

    walker = parent
  }
}

export class NavigationManager {
  focused = null

  _keyFinisher = null

  _resolve (view, navigate) {
    let chain = view

    while (true) {
      chain.waypoint.resolve(chain, navigate)

      const { command, next } = navigate

      if (command !== NavigationContext.DONE) {
        throw Error('resolve() must return a focus candidate.')
      }

      if (next.focusable) {
        return next
      }

      chain = next
      navigate.command = null
      navigate.next = null
    }
  }

  setFocus (view) {
    this._setFocus(view, new NavigationContext(Direction.RIGHT), true)
  }

  _setFocus (view, navigate, sync) {
    let { focused } = this

    if (view === focused) {
      return false
    }

    if (!view || (!view.focusable && !view.waypoint)) {
      throw Error('setFocus requires a focusable view.')
    }

    if (view.waypoint) {
      view = this._resolve(view, navigate)
      if (view === focused) {
        return false
      }
    }

    const { direction } = navigate

    focused && focused.onBlur && focused.onBlur(focused, direction)

    focused = this.focused = view

    focused.onFocus && focused.onFocus(focused, direction)

    sync && syncChildFocus(focused)

    // this.emit('change', previous, this.focused)

    return true
  }

  clearFocus () {
    const focused = this.focused

    focused && focused.onBlur && focused.onBlur(focused)

    this.focused = null

    // this.emit('change', this.focused, null)
  }

  onKeyDown (keyEvent) {
    let walker = this.focused

    if (walker === null) {
      return
    }

    const { direction, key } = keyEvent

    if (direction > 0) {
      const navigate = new NavigationContext(direction)
      let focusChanged

      while (walker != null) {
        const { waypoint } = walker

        if (waypoint) {
          waypoint.navigate(walker, navigate)

          const { command, next } = navigate

          switch (command) {
            case NavigationContext.ABORT:
              this._keyFinisher && this._keyFinisher(key, false)
              return
            case NavigationContext.DONE:
              // sync = false because navigate/resolve take care of updating parent focus
              focusChanged = this._setFocus(next, navigate, false)
              this._keyFinisher && this._keyFinisher(key, focusChanged)
              return
            case NavigationContext.CONTINUE:
              navigate.command = null
              break
            default:
              throw Error('resolve() is required to respond to navigate requests.')
          }
        }

        walker = walker.parent
      }

      focusChanged = navigate.pending ? this._setFocus(navigate.pending, navigate) : false

      this._keyFinisher && this._keyFinisher(keyEvent.key, focusChanged)
    } else {
      walker = this.focused

      while (walker != null && !keyEvent.canceled) {
        walker.onKeyDown && walker.onKeyDown(keyEvent)
        walker = walker.parent
      }
    }
  }

  setKeyFinisher (callback) {
    this._keyFinisher = callback
  }

  destroy () {

  }
}
