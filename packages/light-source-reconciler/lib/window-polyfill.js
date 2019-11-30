/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { performance } from 'perf_hooks'
import { setTimeout, clearTimeout } from './timeout-scope'

// Export a window object containing the minimum functionality required by scheduler. This window is used privately
// by scheduler and is not intended as a general window polyfill.
//
// Note: raf functions are not used by the scheduler, but if the functions are not present an error message prints
// to the console.

export default {
  Date,
  setTimeout,
  clearTimeout,
  performance,
  requestAnimationFrame: () => {},
  cancelAnimationFrame: () => {}
}
