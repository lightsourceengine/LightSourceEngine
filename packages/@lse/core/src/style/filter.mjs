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

import { parseColor, StyleFilter } from '../addon/index.mjs'
import { rgba } from './rgba.mjs'
import { rgb } from './rgb.mjs'

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
