/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'

/**
 * Describes a user interaction with an InputDevice; each event describes a single interaction
 * between the user and a button, axis or keyboard key on the InputDevice. The event type (keydown or keyup) identifies
 * what kind of InputDevice activity occurred.
 */
export class KeyboardEvent extends Event {
  /**
   * Creates a KeyEvent object.
   *
   * @param type {string} Event name.
   * @param cancelable {boolean} Is this event cancelable?
   * @param bubbles {boolean} Does this event bubble through the view graph?
   */
  constructor (type, cancelable, bubbles) {
    super(type, cancelable, bubbles)

    /**
     * Input device that generated this event.
     *
     * @type {InputDevice}
     */
    this.device = null

    /**
     * Mapped key.
     *
     * @type {number}
     */
    this.key = -1

    // /**
    //  * Navigation direction associated with this event's key.
    //  *
    //  * @type {Direction}
    //  */
    // this.direction = Direction.NONE

    /**
     * Indicates whether the key is currently pressed down.
     *
     * @type {boolean}
     */
    this.pressed = false

    /**
     * Indicates whether the key is currently being held down. When the first pressed down event occurs, repeat
     * will be false. If the key is continuously held down, subsequent pressed down events will set repeat to true.
     *
     * @type {boolean}
     */
    this.repeat = false

    /**
     * The mapping object used to translate the device's raw input to a mapped key.
     *
     * @type {Mapping}
     */
    this.mapping = null
  }

  _reset (device, timestamp, mapping, key, direction, pressed = false, repeat = false) {
    this.device = device
    this.key = key
    this.direction = direction
    this.pressed = pressed
    this.repeat = repeat
    this.mapping = mapping

    return super._reset(timestamp)
  }
}
