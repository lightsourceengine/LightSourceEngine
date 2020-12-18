/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { clamp } from '../util/index.js'

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
 */
export const rgb = (r, g, b) => (0xFF000000 | (clamp(r, 0, 255) << 16) | (clamp(g, 0, 255) << 8) | clamp(b, 0, 255)) >>> 0
