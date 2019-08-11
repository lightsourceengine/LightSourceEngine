/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { $phase, $type, $timestamp } from '../util/InternalSymbols'

export class Event {
  static PhaseNone = 0

  static PhaseCapture = 1

  static PhaseBubble = 2

  constructor (type, timestamp) {
    this[$type] = type
    this[$timestamp] = timestamp
    this[$phase] = Event.PhaseNone
  }

  get phase () {
    return this[$phase]
  }

  get name () {
    return Symbol.keyFor(this[$type])
  }

  get type () {
    return this[$type]
  }

  get timestamp () {
    return this[$timestamp]
  }

  stop () {

  }
}
