/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'

const { DeviceConnected, DeviceDisconnected } = EventType

const $device = Symbol.for('device')
const $connected = Symbol.for('connected')

export class DeviceEvent extends Event {
  constructor (device, connected, timestamp) {
    super(connected ? DeviceConnected : DeviceDisconnected, timestamp)

    this[$device] = device
    this[$connected] = connected
  }

  get device () {
    return this[$device]
  }

  get connected () {
    return this[$connected]
  }
}
