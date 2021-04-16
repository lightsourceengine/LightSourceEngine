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

import { clamp } from '../util/index.mjs'

/**
 * Create a style color value from RGB color components.
 *
 * If a component is not an integer, the component will be treated as 0. Otherwise, the component is clamped to
 * [0-255] range.
 *
 * @param {number} r Red color component [0-255]
 * @param {number} g Green color component [0-255]
 * @param {number} b Blue color component [0-255]
 * @returns {number} Style color value in 32-bit RRGGBB format
 *
 * @function rgba
 * @memberof module:@lse/core.$
 */
export const rgb = (r, g, b) => (0xFF000000 | (clamp(r, 0, 255) << 16) | (clamp(g, 0, 255) << 8) | clamp(b, 0, 255)) >>> 0
