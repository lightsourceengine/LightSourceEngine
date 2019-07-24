/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'

/**
 * Describes an InputDevice connection. The event type (connected or disconnected) identifies the connection state
 * of the device.
 */
export class GamepadEvent extends Event {
  /**
   * Creates a DeviceEvent object.
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
  }

  _reset (device, timestamp) {
    this.device = device
    return super._reset(timestamp)
  }
}
