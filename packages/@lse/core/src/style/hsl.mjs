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

import { rgb } from './rgb.mjs'
import { clamp } from '../util/index.mjs'

const toRgbComponent = (p, q, t) => {
  let component

  if (t < 0) {
    t += 1
  }

  if (t > 1) {
    t -= 1
  }

  if (t < 1 / 6) {
    component = p + (q - p) * 6 * t
  } else if (t < (1 / 2)) {
    component = q
  } else if (t < (2 / 3)) {
    component = p + (q - p) * (2 / 3 - t) * 6
  } else {
    component = p
  }

  return Math.round(component * 255)
}

export const hsl = (h, s, l) => {
  s = clamp(s, 0, 1)
  l = clamp(l, 0, 1)

  if (s === 0) {
    const color = Math.round(l * 255)
    return rgb(color, color, color)
  }

  h = clamp(h, 0, 360) / 360

  const q = l < 0.5 ? l * (1 + s) : l + s - l * s
  const p = 2 * l - q

  return rgb(toRgbComponent(p, q, h + (1 / 3)), toRgbComponent(p, q, h), toRgbComponent(p, q, h - (1 / 3)))
}
