/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { StyleTransform } from '../addon/index.js'
import { StyleValue } from './StyleValue.js'
import { StyleTransformSpec } from './StyleTransformSpec.js'

const kUndefinedStyleValue = StyleValue.of(undefined)

/**
 * @method module:@lse/core.$.translate
 */
export const translate = (x, y) => StyleTransformSpec.translate(StyleValue.of(x), StyleValue.of(y))

/**
 * @method module:@lse/core.$.translateX
 */
export const translateX = (x) => translate(x, 0)

/**
 * @method module:@lse/core.$.translateY
 */
export const translateY = (y) => translate(0, y)

/**
 * @method module:@lse/core.$.scale
 */
export const scale = (sx, sy) => StyleTransformSpec.scale(StyleValue.of(sx), StyleValue.of(sy))

/**
 * @method module:@lse/core.$.scaleX
 */
export const scaleX = (sx) => scale(sx, 1)

/**
 * @method module:@lse/core.$.scaleY
 */
export const scaleY = (sy) => scale(1, sy)

/**
 * @method module:@lse/core.$.rotate
 */
export const rotate = (angle) => StyleTransformSpec.rotate(StyleValue.of(angle))

// TODO: Should these be exported?

export const isRotate = (spec) => spec?.transform === StyleTransform.Rotate
export const isTranslate = (spec) => spec?.transform === StyleTransform.Translate
export const isScale = (spec) => spec?.transform === StyleTransform.Scale

export const getRotateAngle = (spec) => isRotate(spec) ? spec.angle : kUndefinedStyleValue
export const getTranslateX = (spec) => isTranslate(spec) ? spec.x : kUndefinedStyleValue
export const getTranslateY = (spec) => isTranslate(spec) ? spec.y : kUndefinedStyleValue
export const getScaleX = (spec) => isScale(spec) ? spec.x : kUndefinedStyleValue
export const getScaleY = (spec) => isScale(spec) ? spec.y : kUndefinedStyleValue
