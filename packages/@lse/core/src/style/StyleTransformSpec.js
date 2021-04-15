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

import { installStyleTransformSpec, StyleTransform, StyleUnit } from '../addon/index.js'
import { emptyArray } from '../util/index.js'
import { StyleValue } from './StyleValue.js'

export class StyleTransformSpec {
  // Note: Use index so the native code can do faster lookups.

  // Used by the native code when a style transform spec should be undefined.
  static [0/* undefined */] = emptyArray;

  // transform type
  [0] = 0;
  // x
  [1] = NaN;
  // y
  [2] = NaN;
  // angle
  [3] = NaN;

  constructor (transform, ...args) {
    switch (transform) {
      case IDENTITY:
        break
      case ROTATE:
        this[3/* angle */] = ensureStyleValue(args[0])
        break
      case TRANSLATE:
      case SCALE:
        this[1/* x */] = ensureStyleValue(args[0])
        this[2/* x */] = ensureStyleValue(args[1])
        break
      default:
        throwInvalidTransform()
    }

    this[0/* transform */] = transform
    Object.freeze(this)
  }

  get transform () {
    return this[0]
  }

  get x () {
    return this[1]
  }

  get y () {
    return this[2]
  }

  get angle () {
    return this[3]
  }

  isIdentity () {
    return this.transform === IDENTITY
  }

  isRotate () {
    return this.transform === ROTATE
  }

  isScale () {
    return this.transform === SCALE
  }

  isTranslate () {
    return this.transform === TRANSLATE
  }

  static rotate (angle) {
    const value = angle instanceof StyleValue ? angle : StyleValue.of(angle)

    switch (value?.unit) {
      case StyleUnit.POINT:
      case StyleUnit.RADIAN:
      case StyleUnit.GRADIAN:
      case StyleUnit.DEGREE:
      case StyleUnit.TURN:
        return new StyleTransformSpec(ROTATE, value)
    }
  }

  static identity () {
    return kIdentity
  }

  static translate (x, y) {
    const xValue = x instanceof StyleValue ? x : StyleValue.of(x)
    const yValue = y instanceof StyleValue ? y : StyleValue.of(y)

    if (isValidTranslateArg(xValue) && isValidTranslateArg(yValue)) {
      return new StyleTransformSpec(TRANSLATE, xValue, yValue)
    }
  }

  static scale (x, y) {
    const xValue = x instanceof StyleValue ? x : StyleValue.of(x)
    const yValue = y instanceof StyleValue ? y : StyleValue.of(y)

    if (isValidScaleArg(xValue) && isValidTranslateArg(yValue)) {
      return new StyleTransformSpec(SCALE, xValue, yValue)
    }
  }
}

const { IDENTITY, ROTATE, SCALE, TRANSLATE } = StyleTransform
const kIdentity = new StyleTransformSpec(IDENTITY)

const isValidTranslateArg = ({ unit } = {}) => {
  switch (unit) {
    case StyleUnit.POINT:
    case StyleUnit.PERCENT:
    case StyleUnit.VIEWPORT_WIDTH:
    case StyleUnit.VIEWPORT_HEIGHT:
    case StyleUnit.VIEWPORT_MIN:
    case StyleUnit.VIEWPORT_MAX:
    case StyleUnit.REM:
      return true
    default:
      return false
  }
}

const isValidScaleArg = (arg) => {
  return arg?.unit === StyleUnit.POINT
}

const throwInvalidTransform = (transform) => {
  throw Error(`Invalid transform: ${transform}`)
}

const ensureStyleValue = (value) => {
  if (!(value instanceof StyleValue)) {
    throw Error('Expected StyleValue.')
  }
  return value
}

// Install this class in the environment so the native Style parsing can use it.
installStyleTransformSpec(StyleTransformSpec)
