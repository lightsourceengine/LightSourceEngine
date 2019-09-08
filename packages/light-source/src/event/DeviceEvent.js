/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'

const { DeviceConnected, DeviceDisconnected } = EventType

export class DeviceEvent extends Event {
  constructor (device, connected, timestamp) {
    super(connected ? DeviceConnected : DeviceDisconnected, timestamp)

    this.device = device
    this.connected = connected
  }
}
