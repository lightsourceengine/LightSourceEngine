/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'

/**
 * Describes a user interaction with an InputDevice; each event describes a single interaction
 * between the user and a raw, unmapped axis on the InputDevice. The event type (axismove) identifies
 * what kind of InputDevice activity occurred.
 */
export class GamepadAxisEvent extends Event {
  /**
   * Creates a DeviceAxisEvent object.
   *
   * @param type {string} Event name.
   */
  constructor (type) {
    super(type, false, false)

    /**
     * The InputDevice that generated this event.
     *
     * @type {InputDevice}
     */
    this.device = null

    /**
     * The axis index associated with this event.
     *
     * @type {number}
     */
    this.axis = -1

    /**
     * The current value of the axis, a number between -1 and 1, inclusive.
     *
     * @type {number}
     */
    this.value = 0
  }

  _reset (device, timestamp, axis, value) {
    this.device = device
    this.axis = axis
    this.value = value

    return super._reset(timestamp)
  }
}
