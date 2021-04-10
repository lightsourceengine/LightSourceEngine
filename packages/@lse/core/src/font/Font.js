/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
