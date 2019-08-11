/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Event } from './Event'
import { EventType } from './EventType'
import { $key, $pressed, $repeat, $mapping, $source } from '../util/InternalSymbols'

const { KeyUp, KeyDown } = EventType

export class KeyEvent extends Event {
  constructor (key, pressed, repeat, mapping, source, timestamp) {
    super(pressed ? KeyUp : KeyDown, timestamp)

    this[$key] = key
    this[$pressed] = pressed
    this[$repeat] = repeat
    this[$mapping] = mapping
    this[$source] = source
  }

  get key () {
    return this[$key]
  }

  get pressed () {
    return this[$pressed]
  }

  get repeat () {
    return this[$repeat]
  }

  get mapping () {
    return this[$mapping]
  }

  get source () {
    return this[$source]
  }
}
