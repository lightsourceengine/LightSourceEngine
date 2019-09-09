/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { NONE } from './EventPhase'
import { symbolKeyFor } from '../util'

export class Event {
  constructor (type, timestamp) {
    this.phase = NONE
    this.type = type
    this.timestamp = timestamp
    this.cancelled = false
  }

  get name () {
    return symbolKeyFor(this.type)
  }

  stop () {
    this.cancelled = true
  }
}
