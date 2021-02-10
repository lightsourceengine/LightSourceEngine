/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
      case Identity:
        break
      case Rotate:
        this[3/* angle */] = ensureStyleValue(args[0])
        break
      case Translate:
      case Scale:
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
    return this.transform === Identity
  }

  isRotate () {
    return this.transform === Rotate
  }

  isScale () {
    return this.transform === Scale
  }

  isTranslate () {
    return this.transform === Translate
  }

  static rotate (angle) {
    const value = angle instanceof StyleValue ? angle : StyleValue.of(angle)

    switch (value?.unit) {
      case StyleUnit.Point:
      case StyleUnit.Radian:
      case StyleUnit.Gradian:
      case StyleUnit.Degree:
      case StyleUnit.Turn:
        return new StyleTransformSpec(Rotate, value)
    }
  }

  static identity () {
    return kIdentity
  }

  static translate (x, y) {
    const xValue = x instanceof StyleValue ? x : StyleValue.of(x)
    const yValue = y instanceof StyleValue ? y : StyleValue.of(y)

    if (isValidTranslateArg(xValue) && isValidTranslateArg(yValue)) {
      return new StyleTransformSpec(Translate, xValue, yValue)
    }
  }

  static scale (x, y) {
    const xValue = x instanceof StyleValue ? x : StyleValue.of(x)
    const yValue = y instanceof StyleValue ? y : StyleValue.of(y)

    if (isValidScaleArg(xValue) && isValidTranslateArg(yValue)) {
      return new StyleTransformSpec(Scale, xValue, yValue)
    }
  }
}

const { Identity, Rotate, Scale, Translate } = StyleTransform
const kIdentity = new StyleTransformSpec(Identity)

const isValidTranslateArg = ({ unit } = {}) => {
  switch (unit) {
    case StyleUnit.Point:
    case StyleUnit.Percent:
    case StyleUnit.ViewportWidth:
    case StyleUnit.ViewportHeight:
    case StyleUnit.ViewportMin:
    case StyleUnit.ViewportMax:
    case StyleUnit.RootEm:
      return true
    default:
      return false
  }
}

const isValidScaleArg = (arg) => {
  return arg?.unit === StyleUnit.Point
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
