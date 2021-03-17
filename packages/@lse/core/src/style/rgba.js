/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { parseColor } from '../addon/index.js'
import { clamp, isNumber } from '../util/index.js'
import { rgb } from './rgb.js'

/**
 * Create a style color value from RGB color components and an opacity (alpha component).
 *
 * @param args Expressed as (color, opacity) or (red, green, blue, opacity). color is a style color string, hex color
 * or named color, or a RRGGBB color value. red, green and blue color components are numbers in the range [0-255]. The
 * opacity value is a number [0,1], where 0 is transparent and 1 is opaque. If the args are not in the above format,
 * opaque black is returned
 * @returns {number} Style color value in 33-bit 1AARRGGBB format. The 33 bit is set as a hint to the renderer to
 * honor the alpha component of the color.
 *
 * @function rgba
 * @memberof module:@lse/core.$
 */
export const rgba = (...args) => {
  let a
  let color

  if (args.length === 2) {
    a = args[1]
    color = ((typeof args[0] === 'string') ? parseColor(args[0]) : args[0]) & 0xFFFFFF
  } else if (args.length === 4) {
    a = args[3]
    color = rgb(args[0], args[1], args[2]) & 0xFFFFFF
  } else {
    a = 1
    color = 0
  }

  a = isNumber(a) ? clamp(a, 0, 1) * 255 : 255

  return (((a << 24) | color) >>> 0)
}
