/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { parseColor, StyleFilter } from '../addon/index.js'
import { rgba } from './rgba.js'
import { rgb } from './rgb.js'

const kFlipH = Object.freeze([StyleFilter.FLIP_H])
const kFlipV = Object.freeze([StyleFilter.FLIP_V])

/**
 * @method module:@lse/core.$.flipH
 */
export const flipH = () => kFlipH

/**
 * @method module:@lse/core.$.flipV
 */
export const flipV = () => kFlipV

/**
 * @method module:@lse/core.$.tint
 */
export const tint = (...args) => {
  let color

  switch (args.length) {
    case 1:
      color = parseColor(args[0])
      break
    case 3:
      color = rgb(...args)
      break
    case 2:
    case 4:
      color = rgba(...args)
      break
    default:
      color = 0xFFFFFFFF
      break
  }

  return Object.freeze([StyleFilter.TINT, color])
}
