/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { EventTarget } from '../event/EventTarget.js'
import { EventName } from '../event/EventName.js'

/**
 * Font face.
 *
 * Contains information about a font face added to the FontManager. If the font is in the loading state, this
 * object will broadcast an 'on-status' event when the font is ready.
 *
 * @memberof module:@lse/core
 * @extends module:@lse/core.EventTarget
 * @hideconstructor
 */
class Font extends EventTarget {
  _id = -1
  _family = ''
  _style = ''
  _weight = ''
  $status = ''

  constructor (id, family, style, weight, status) {
    super([EventName.status])

    this._id = id
    this._family = family
    this._style = style
    this._weight = weight
    this.$status = status
  }

  /**
   * Resource ID of the font.
   * @returns {number}
   */
  get id () {
    return this._id
  }

  /**
   * Font family name. Used for fontFamily style property.
   * @returns {string}
   */
  get family () {
    return this._family
  }

  /**
   * Font style supported by this font face.
   * @returns {string}
   */
  get style () {
    return this._style
  }

  /**
   * Font weight supported by this font face.
   * @returns {string}
   */
  get weight () {
    return this._weight
  }

  /**
   * Loading status of this font face.
   *
   * If the status is loading, subscribe to an 'on-status' event to be notified when the font is ready or error.
   *
   * @returns {string}
   */
  get status () {
    return this.$status
  }
}

export { Font }
