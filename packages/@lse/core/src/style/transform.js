/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { StyleValue } from './StyleValue.js'
import { StyleTransformSpec } from './StyleTransformSpec.js'

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
