/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Style } from '../addon'
import { isNumber } from '../util'

const TRANSLATE_VALUE_REGEX = /^(-?\d+\.?\d*)(px|%)$/
const TRANSLATE_UNIT_TO_ENUM = new Map([
  ['px', Style.UnitPoint],
  ['%', Style.UnitPercent]
])
const ROTATE_VALUE_REGEX = /^(-?\d+\.?\d*)(deg|rad|grad|turn)$/
const ROTATE_UNIT_TO_ENUM = new Map([
  ['deg', Style.UnitDegree],
  ['rad', Style.UnitRadian],
  ['grad', Style.UnitGradian],
  ['turn', Style.UnitTurn]
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

  return [undefined, Style.UnitUndefined]
}

export const translate = (x, y) => {
  const [xValue, xUnit] = toStyleValue(x, Style.UnitPoint, TRANSLATE_VALUE_REGEX, TRANSLATE_UNIT_TO_ENUM)
  const [yValue, yUnit] = toStyleValue(y, Style.UnitPoint, TRANSLATE_VALUE_REGEX, TRANSLATE_UNIT_TO_ENUM)

  if (typeof xValue === 'undefined' || typeof yValue === 'undefined') {
    return
  }

  return Float32Array.of(Style.TransformTranslate, xValue, xUnit, yValue, yUnit)
}

export const translateX = (x) => translate(x, 0)
export const translateY = (y) => translate(0, y)

export const scale = (sx, sy) => {
  if (!isNumber(sx) || !isNumber(sy)) {
    return
  }

  return Float32Array.of(Style.TransformScale, sx, Style.UnitPoint, sy, Style.UnitPoint)
}

export const scaleX = (sx) => scale(sx, 1)
export const scaleY = (sy) => scale(1, sy)

export const rotate = (angle) => {
  const [aValue, aUnit] = toStyleValue(angle, Style.UnitRadian, ROTATE_VALUE_REGEX, ROTATE_UNIT_TO_ENUM)

  if (typeof aValue === 'undefined') {
    return
  }

  return Float32Array.of(Style.TransformRotate, aValue, aUnit)
}

export const isRotate = (transform) => transform instanceof Float32Array && transform[0] === Style.TransformRotate
export const isTranslate = (transform) => transform instanceof Float32Array && transform[0] === Style.TransformTranslate
export const isScale = (transform) => transform instanceof Float32Array && transform[0] === Style.TransformScale

export const getRotateAngle = (transform) => isRotate(transform) ? [transform[1], transform[2]] : []
export const getTranslateX = (transform) => isTranslate(transform) ? [transform[1], transform[2]] : []
export const getTranslateY = (transform) => isTranslate(transform) ? [transform[3], transform[4]] : []
export const getScaleX = (transform) => isScale(transform) ? [transform[1], transform[2]] : []
export const getScaleY = (transform) => isScale(transform) ? [transform[3], transform[4]] : []