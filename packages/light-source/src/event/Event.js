/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */
import assert from 'assert'

/**
 * Base class for events that take place in the view graph.
 */
export class Event {
  /**
   * Creates an Event object.
   *
   * @param type {string} Event name.
   * @param cancelable {boolean} Is this event cancelable?
   * @param bubbles {boolean} Does this event bubble through the view graph?
   */
  constructor (type, cancelable, bubbles) {
    assert(typeof type === 'string', 'Event type must be a string.')

    /**
     * The name of the event.
     *
     * @property {string}
     * @name Event#type
     */
    Object.defineProperty(this, 'type', {
      value: type,
      writable: false
    })

    /**
     * Indicates whether the event can be canceled.
     *
     * @property {boolean}
     * @name Event#cancelable
     */
    Object.defineProperty(this, 'cancelable', {
      value: !!cancelable,
      writable: false
    })

    /**
     * Indicates whether the event bubbles up through the view graph or not.
     *
     * @property {boolean}
     * @name Event#bubbles
     */
    Object.defineProperty(this, 'bubbles', {
      value: !!bubbles,
      writable: false
    })

    this._canceled = false
    this._timestamp = 0
  }

  cancel () {
    this.cancelable && (this._canceled = true)
  }

  /**
   * Was cancel() called on this event?
   *
   * @returns {boolean}
   */
  get canceled () {
    return this._canceled
  }

  /**
   * The time which the event was created in milliseconds. This value is a high resolution timestamp relative to
   * performance.timeOrigin.
   *
   * @returns {number}
   */
  get timestamp () {
    return this._timestamp
  }

  _reset (timestamp) {
    this._canceled = false
    this._timestamp = timestamp

    return this
  }
}
