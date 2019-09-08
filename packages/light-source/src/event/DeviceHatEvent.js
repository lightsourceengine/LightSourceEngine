/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'

const { DeviceHatDown, DeviceHatUp } = EventType

export class DeviceHatEvent extends Event {
  constructor (device, hatIndex, hatValue, pressed, repeat, timestamp) {
    super(pressed ? DeviceHatDown : DeviceHatUp, timestamp)

    this.device = device
    this.hat = hatIndex
    this.value = hatIndex
    this.pressed = pressed
    this.repeat = repeat
  }
}
