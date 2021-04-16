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

import { StyleValue } from './StyleValue.mjs'
import { StyleTransformSpec } from './StyleTransformSpec.mjs'

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
