/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { FixedListWaypoint } from './FixedListWaypoint.js'

/**
 * @method module:@lse/core.waypoint
 */
export const waypoint = (tag) => {
  return new FixedListWaypoint(tag)
}
