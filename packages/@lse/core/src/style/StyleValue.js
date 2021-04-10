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

import { installStyleValue, parseValue, StyleUnit } from '../addon/index.js'

/**
 * A style value object.
 *
 * @memberof module:@lse/core
 * @hideconstructor
 */
class StyleValue {
  // Note: Use index so the native code can do faster lookups.

  // Used by the native code when a style value should be undefined.
  static [0] = new StyleValue(0, StyleUnit.Undefined);

  // value
  [0] = 0.0;

  // unit type
  [1] = 0;

  constructor (value, unit) {
    this[0] = value || 0
    this[1] = unit || StyleUnit.Undefined
    Object.freeze(this)
  }

  get value () {
    return this[0]
  }

  get unit () {
    return this[1]
  }

  isUndefined () {
    return this.unit === StyleUnit.Undefined
  }

  isPoint () {
    return this.unit === StyleUnit.Point
  }

  isPercent () {
    return this.unit === StyleUnit.Percent
  }

  isViewportWidth () {
    return this.unit === StyleUnit.ViewportWidth
  }

  isViewportHeight () {
    return this.unit === StyleUnit.ViewportHeight
  }

  isViewportMin () {
    return this.unit === StyleUnit.ViewportMin
  }

  isViewportMax () {
    return this.unit === StyleUnit.ViewportMax
  }

  isAuto () {
    return this.unit === StyleUnit.Auto
  }

  isAnchor () {
    return this.unit === StyleUnit.Anchor
  }

  isRootEm () {
    return this.unit === StyleUnit.RootEm
  }

  isRadian () {
    return this.unit === StyleUnit.Radian
  }

  isDegree () {
    return this.unit === StyleUnit.Degree
  }

  isGradian () {
    return this.unit === StyleUnit.Gradian
  }

  isTurn () {
    return this.unit === StyleUnit.Turn
  }

  static of (value) {
    return parseValue(value)
  }
}

// Install this class in the environment so the native Style parsing can use it.
installStyleValue(StyleValue)

export { StyleValue }
