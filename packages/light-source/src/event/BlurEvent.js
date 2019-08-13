/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { EventType } from './EventType'
import { Event } from './Event'

const { Blur } = EventType

export class BlurEvent extends Event {
  constructor (timestamp) {
    super(Blur, timestamp)
  }
}
