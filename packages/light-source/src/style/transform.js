/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { StyleUnit, StyleTransform } from '../addon'
import { isNumber } from '../util'

const TRANSLATE_VALUE_REGEX = /^(-?\d+\.?\d*)(px|%)$/
const TRANSLATE_UNIT_TO_ENUM = new Map([
  ['px', StyleUnit.Point],
  ['%', StyleUnit.Percent]
])
const ROTATE_VALUE_REGEX = /^(-?\d+\.?\d*)(deg|rad|grad|turn)$/
const ROTATE_UNIT_TO_ENUM = new Map([
  ['deg', StyleUnit.Degree],
  ['rad', StyleUnit.Radian],
  ['grad', StyleUnit.Gradian],
  ['turn', StyleUnit.Turn]
])

const toStyleValue = (raw, numUnit, regex, toEnum) => {
  if (isNumber(raw)) {
    return [raw, numUnit]
  } else if (typeof raw === 'string') {
    const match = regex.exec(raw)

    if (match) {
      return [match[1], toEnum.get(match[2])]
    }
  }

  return [undefined, StyleUnit.Undefined]
}

export const translate = (x, y) => {
  const [xValue, xUnit] = toStyleValue(x, StyleUnit.Point, TRANSLATE_VALUE_REGEX, TRANSLATE_UNIT_TO_ENUM)
  const [yValue, yUnit] = toStyleValue(y, StyleUnit.Point, TRANSLATE_VALUE_REGEX, TRANSLATE_UNIT_TO_ENUM)

  if (typeof xValue === 'undefined' || typeof yValue === 'undefined') {
    return
  }

  return Float32Array.of(StyleTransform.Translate, xValue, xUnit, yValue, yUnit)
}

export const translateX = (x) => translate(x, 0)
export const translateY = (y) => translate(0, y)

export const scale = (sx, sy) => {
  if (!isNumber(sx) || !isNumber(sy)) {
    return
  }

  return Float32Array.of(StyleTransform.Scale, sx, StyleUnit.Point, sy, StyleUnit.Point)
}

export const scaleX = (sx) => scale(sx, 1)
export const scaleY = (sy) => scale(1, sy)

export const rotate = (angle) => {
  const [aValue, aUnit] = toStyleValue(angle, StyleUnit.Radian, ROTATE_VALUE_REGEX, ROTATE_UNIT_TO_ENUM)

  if (typeof aValue === 'undefined') {
    return
  }

  return Float32Array.of(StyleTransform.Rotate, aValue, aUnit)
}

export const isRotate = (transform) => transform instanceof Float32Array && transform[0] === StyleTransform.Rotate
export const isTranslate = (transform) => transform instanceof Float32Array && transform[0] === StyleTransform.Translate
export const isScale = (transform) => transform instanceof Float32Array && transform[0] === StyleTransform.Scale

export const getRotateAngle = (transform) => isRotate(transform) ? [transform[1], transform[2]] : []
export const getTranslateX = (transform) => isTranslate(transform) ? [transform[1], transform[2]] : []
export const getTranslateY = (transform) => isTranslate(transform) ? [transform[3], transform[4]] : []
export const getScaleX = (transform) => isScale(transform) ? [transform[1], transform[2]] : []
export const getScaleY = (transform) => isScale(transform) ? [transform[3], transform[4]] : []
