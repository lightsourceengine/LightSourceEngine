/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'
import { $device, $button, $pressed, $repeat } from '../util/InternalSymbols'

const { DeviceButtonDown, DeviceButtonUp } = EventType

export class DeviceButtonEvent extends Event {
  constructor (device, button, pressed, repeat, timestamp) {
    super(pressed ? DeviceButtonDown : DeviceButtonUp, timestamp)

    this[$device] = device
    this[$button] = button
    this[$pressed] = pressed
    this[$repeat] = repeat
  }

  get device () {
    return this[$repeat]
  }

  get button () {
    return this[$button]
  }

  get pressed () {
    return this[$pressed]
  }

  get repeat () {
    return this[$repeat]
  }
}
