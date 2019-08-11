/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'

const { DeviceAxisMotion } = EventType

const $device = Symbol.for('device')
const $axis = Symbol.for('axis')
const $value = Symbol.for('value')

export class DeviceAxisEvent extends Event {
  constructor (device, axis, value, timestamp) {
    super(DeviceAxisMotion, timestamp)

    this[$device] = device
    this[$axis] = axis
    this[$value] = value
  }

  get device () {
    return this[$device]
  }

  get axis () {
    return this[$axis]
  }

  get value () {
    return this[$value]
  }
}
