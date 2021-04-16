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

import chai from 'chai'
import { StyleTransform } from '../../src/addon/index.mjs'
import { StyleTransformSpec } from '../../src/style/StyleTransformSpec.mjs'
import { StyleValue } from '../../src/style/StyleValue.mjs'

const { assert } = chai

const invalidStyleValues = ['test', '', {}, [], null, undefined]

// StyleTransformSpec defined in JSStyleTransformSpec.cc
describe('StyleTransformSpec', () => {
  describe('constructor', () => {
    it('should construct an identity StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.IDENTITY)

      assert.equal(xform.transform, StyleTransform.IDENTITY)
      assert.isTrue(xform.isIdentity())
      assert.isNaN(xform.x)
      assert.isNaN(xform.y)
      assert.isNaN(xform.angle)
      assert.isFrozen(xform)
    })
    it('should construct a rotate StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.ROTATE, StyleValue.of('90deg'))

      assert.equal(xform.transform, StyleTransform.ROTATE)
      assert.isTrue(xform.isRotate())
      assert.isNaN(xform.x)
      assert.isNaN(xform.y)
      assert.deepEqual(xform.angle, StyleValue.of('90deg'))
      assert.isFrozen(xform)
    })
    it('should construct a translate StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.TRANSLATE, StyleValue.of(10), StyleValue.of(15))

      assert.equal(xform.transform, StyleTransform.TRANSLATE)
      assert.isTrue(xform.isTranslate())
      assert.deepEqual(xform.x, StyleValue.of(10))
      assert.deepEqual(xform.y, StyleValue.of(15))
      assert.isNaN(xform.angle)
      assert.isFrozen(xform)
    })
    it('should construct a scale StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.SCALE, StyleValue.of(10), StyleValue.of(15))

      assert.equal(xform.transform, StyleTransform.SCALE)
      assert.isTrue(xform.isScale())
      assert.deepEqual(xform.x, StyleValue.of(10))
      assert.deepEqual(xform.y, StyleValue.of(15))
      assert.isNaN(xform.angle)
      assert.isFrozen(xform)
    })
    it('should throw error when no arguments', () => {
      assert.throws(() => new StyleTransformSpec())
    })
    it('should throw error when transform is invalid', () => {
      for (const input of [[-1, 999, NaN, '', {}, [], null, undefined]]) {
        assert.throws(() => new StyleTransformSpec(input))
      }
    })
    it('should throw error when rotate has no angle', () => {
      assert.throws(() => new StyleTransformSpec(StyleTransform.ROTATE))
    })
    it('should throw error when rotate angle is invalid', () => {
      for (const value of invalidStyleValues) {
        assert.throws(() => new StyleTransformSpec(StyleTransform.ROTATE, value))
      }
    })
    it('should throw error when translate is missing args', () => {
      assert.throws(() => new StyleTransformSpec(StyleTransform.TRANSLATE))
    })
    it('should throw error when translate is missing y arg', () => {
      assert.throws(() => new StyleTransformSpec(StyleTransform.TRANSLATE, StyleValue.of(10)))
    })
    it('should throw error when translate args are invalid', () => {
      for (const value of invalidStyleValues) {
        assert.throws(() => new StyleTransformSpec(StyleTransform.TRANSLATE, value, value))
      }
    })
    it('should throw error when scale is missing args', () => {
      assert.throws(() => new StyleTransformSpec(StyleTransform.SCALE))
    })
    it('should throw error when scale is missing y arg', () => {
      assert.throws(() => new StyleTransformSpec(StyleTransform.SCALE, StyleValue.of(10)))
    })
    it('should throw error when scale args are invalid', () => {
      for (const value of invalidStyleValues) {
        assert.throws(() => new StyleTransformSpec(StyleTransform.SCALE, value, value))
      }
    })
  })
  describe('rotate()', () => {
    it('should return rotate transform for number angle', () => {
      const spec = StyleTransformSpec.rotate(90)

      assert.isTrue(spec.isRotate())
    })
    it('should return rotate transform for StyleValue angle', () => {
      const spec = StyleTransformSpec.rotate(StyleValue.of(90))

      assert.isTrue(spec.isRotate())
    })
    it('should return undefined when no angle is passed', () => {
      assert.isUndefined(StyleTransformSpec.rotate())
    })
    it('should return undefined when angle is an unsupported unit', () => {
      assert.isUndefined(StyleTransformSpec.rotate(StyleValue.of('1rem')))
    })
    it('should return undefined when passed an invalid angle', () => {
      for (const value of invalidStyleValues) {
        assert.isUndefined(StyleTransformSpec.rotate(StyleValue.of(value)))
      }
    })
  })
  describe('translate()', () => {
    it('should return translate transform for x,y', () => {
      const spec = StyleTransformSpec.translate(10, 11)

      assert.isTrue(spec.isTranslate())
      assert.deepEqual(spec.x, StyleValue.of(10))
      assert.deepEqual(spec.y, StyleValue.of(11))
    })
    it('should return translate transform for x,y as StyleValue', () => {
      const spec = StyleTransformSpec.translate(StyleValue.of(10), StyleValue.of(11))

      assert.isTrue(spec.isTranslate())
      assert.deepEqual(spec.x, StyleValue.of(10))
      assert.deepEqual(spec.y, StyleValue.of(11))
    })
    it('should return undefined when no x or y is passed', () => {
      assert.isUndefined(StyleTransformSpec.translate())
    })
    it('should return undefined when no y is passed', () => {
      assert.isUndefined(StyleTransformSpec.translate(10))
    })
    it('should return undefined when x,y is an unsupported unit', () => {
      assert.isUndefined(StyleTransformSpec.translate(StyleValue.of('1grad'), StyleValue.of('1grad')))
    })
    it('should return undefined when passed an invalid x', () => {
      for (const value of invalidStyleValues) {
        assert.isUndefined(StyleTransformSpec.translate(StyleValue.of(value), 11))
      }
    })
    it('should return undefined when passed an invalid y', () => {
      for (const value of invalidStyleValues) {
        assert.isUndefined(StyleTransformSpec.translate(11, StyleValue.of(value)))
      }
    })
  })
  describe('scale()', () => {
    it('should return scale transform for x,y', () => {
      const spec = StyleTransformSpec.scale(2, 3)

      assert.isTrue(spec.isScale())
      assert.deepEqual(spec.x, StyleValue.of(2))
      assert.deepEqual(spec.y, StyleValue.of(3))
    })
    it('should return scale transform for x,y as StyleValue', () => {
      const spec = StyleTransformSpec.scale(StyleValue.of(2), StyleValue.of(3))

      assert.isTrue(spec.isScale())
      assert.deepEqual(spec.x, StyleValue.of(2))
      assert.deepEqual(spec.y, StyleValue.of(3))
    })
    it('should return undefined when no x or y is passed', () => {
      assert.isUndefined(StyleTransformSpec.scale())
    })
    it('should return undefined when no y is passed', () => {
      assert.isUndefined(StyleTransformSpec.scale(10))
    })
    it('should return undefined when x,y is an unsupported unit', () => {
      assert.isUndefined(StyleTransformSpec.scale(StyleValue.of('1grad'), StyleValue.of('1grad')))
    })
    it('should return undefined when passed an invalid x', () => {
      for (const value of invalidStyleValues) {
        assert.isUndefined(StyleTransformSpec.scale(StyleValue.of(value), 11))
      }
    })
    it('should return undefined when passed an invalid y', () => {
      for (const value of invalidStyleValues) {
        assert.isUndefined(StyleTransformSpec.scale(11, StyleValue.of(value)))
      }
    })
  })
  describe('identity()', () => {
    it('should return identity transform', () => {
      const spec = StyleTransformSpec.identity()

      assert.isTrue(spec.isIdentity())
    })
  })
})
