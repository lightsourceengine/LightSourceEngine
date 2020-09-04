/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { StyleTransform, StyleTransformSpec, StyleValue } from '../addon'

const kUndefinedStyleValue = StyleValue.of(undefined)

export const translate = (x, y) => StyleTransformSpec.translate(StyleValue.of(x), StyleValue.of(y))

export const translateX = (x) => translate(x, 0)
export const translateY = (y) => translate(0, y)

export const scale = (sx, sy) => StyleTransformSpec.scale(StyleValue.of(sx), StyleValue.of(sy))

export const scaleX = (sx) => scale(sx, 1)
export const scaleY = (sy) => scale(1, sy)

export const rotate = (angle) => StyleTransformSpec.rotate(StyleValue.of(angle))

export const isRotate = (spec) => spec?.transform === StyleTransform.Rotate
export const isTranslate = (spec) => spec?.transform === StyleTransform.Translate
export const isScale = (spec) => spec?.transform === StyleTransform.Scale

export const getRotateAngle = (spec) => isRotate(spec) ? spec.angle : kUndefinedStyleValue
export const getTranslateX = (spec) => isTranslate(spec) ? spec.x : kUndefinedStyleValue
export const getTranslateY = (spec) => isTranslate(spec) ? spec.y : kUndefinedStyleValue
export const getScaleX = (spec) => isScale(spec) ? spec.x : kUndefinedStyleValue
export const getScaleY = (spec) => isScale(spec) ? spec.y : kUndefinedStyleValue
