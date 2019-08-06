/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { performance } from 'perf_hooks'

/*
 * Add performance to the global scope to mirror the browser environment so modules (scheduler and react-reconciler)
 * can access the high resolution timer (global.performance.now).
 */

if (!global.performance) {
  global.performance = performance
}
