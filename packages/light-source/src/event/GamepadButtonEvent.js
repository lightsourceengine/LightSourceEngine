/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'

/**
 * Describes a user interaction with an InputDevice; each event describes a single interaction
 * between the user and a raw, unmapped button on the InputDevice. The event type (buttondown or buttonup) identifies
 * what kind of InputDevice activity occurred.
 */
export class GamepadButtonEvent extends Event {
  /**
   * Creates a DeviceButtonEvent object.
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
     * The raw button the user interacted with.
     *
     * @type {number}
     */
    this.button = -1

    /**
     * Indicates whether the raw button is currently pressed down.
     *
     * @type {boolean}
     */
    this.pressed = false

    /**
     * Indicates whether the raw button is currently being held down. When the first pressed down event occurs, repeat
     * will be false. If the raw button is continuously held down, subsequent pressed down events will set repeat to true.
     *
     * @type {boolean}
     */
    this.repeat = false
  }

  _reset (device, timestamp, button, pressed = false, repeat = false) {
    this.device = device
    this.button = button
    this.pressed = pressed
    this.repeat = repeat

    return super._reset(timestamp)
  }
}
