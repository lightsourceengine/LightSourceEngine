/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import { performance } from 'perf_hooks'
import { setTimeout, clearTimeout } from './timeout-scope.js'

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
