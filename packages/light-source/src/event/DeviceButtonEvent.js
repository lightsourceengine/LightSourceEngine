/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'

const { DeviceButtonDown, DeviceButtonUp } = EventType

const $buttonState = Symbol.for('buttonState')
const buttonMask = 0xFFFF
const pressedFlag = 1 << 16
const repeatFlag = 1 << 17

export class DeviceButtonEvent extends Event {
  constructor (device, button, pressed, repeat, timestamp) {
    super(pressed ? DeviceButtonDown : DeviceButtonUp, timestamp)

    this.device = device
    this[$buttonState] = (button & buttonMask) | (pressed ? pressedFlag : 0) | (repeat ? repeatFlag : 0)
  }

  get button () {
    return this[$buttonState] & buttonMask
  }

  get pressed () {
    return (this[$buttonState] & pressedFlag) !== 0
  }

  get repeat () {
    return (this[$buttonState] & repeatFlag) !== 0
  }
}
