/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'

const { KeyUp, KeyDown } = EventType

export class KeyEvent extends Event {
  constructor (key, pressed, repeat, mapping, direction, source, timestamp) {
    super(pressed ? KeyUp : KeyDown, timestamp)

    this.key = key
    this.pressed = pressed
    this.repeat = repeat
    this.mapping = mapping
    this.source = source
    this.direction = direction
  }
}
