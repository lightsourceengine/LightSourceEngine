/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { StyleUnit, StyleTransformSpec, StyleTransform, StyleValue } from '../../src/addon/index.js'

const { assert } = chai

// StyleTransformSpec defined in JSStyleTransformSpec.cc
describe('StyleTransformSpec', () => {
  describe('constructor', () => {
    it('should construct an identity StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.Identity)

      assert.equal(xform.transform, StyleTransform.Identity)
      assert.isUndefined(xform.x)
      assert.isUndefined(xform.y)
      assert.isUndefined(xform.angle)
      assert.isFalse(xform.isUndefined())
      assert.isFrozen(xform)
    })
    it('should construct a rotate StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.Rotate, new StyleValue(90, StyleUnit.Degree))

      assert.equal(xform.transform, StyleTransform.Rotate)
      assert.isUndefined(xform.x)
      assert.isUndefined(xform.y)
      assert.deepEqual(xform.angle, { value: 90, unit: StyleUnit.Degree })
      assert.isFalse(xform.isUndefined())
      assert.isFrozen(xform)
    })
    it('should construct a translate StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.Translate,
        new StyleValue(10, StyleUnit.Point), new StyleValue(15, StyleUnit.Point))

      assert.equal(xform.transform, StyleTransform.Translate)
      assert.deepEqual(xform.x, { value: 10, unit: StyleUnit.Point })
      assert.deepEqual(xform.y, { value: 15, unit: StyleUnit.Point })
      assert.isUndefined(xform.angle)
      assert.isFalse(xform.isUndefined())
      assert.isFrozen(xform)
    })
    it('should construct a scale StyleTransformSpec', () => {
      const xform = new StyleTransformSpec(StyleTransform.Scale,
        new StyleValue(10, StyleUnit.Point), new StyleValue(15, StyleUnit.Point))

      assert.equal(xform.transform, StyleTransform.Scale)
      assert.deepEqual(xform.x, { value: 10, unit: StyleUnit.Point })
      assert.deepEqual(xform.y, { value: 15, unit: StyleUnit.Point })
      assert.isUndefined(xform.angle)
      assert.isFalse(xform.isUndefined())
      assert.isFrozen(xform)
    })
    it('should construct an undefined StyleTransformSpec', () => {
      const xform = new StyleTransformSpec()

      assert.equal(xform.transform, -1)
      assert.isUndefined(xform.x)
      assert.isUndefined(xform.y)
      assert.isUndefined(xform.angle)
      assert.isTrue(xform.isUndefined())
      assert.isFrozen(xform)
    })
  })
  describe('validate()', () => {
    it('should return true for a rotate transform', () => {
      const spec = new StyleTransformSpec(StyleTransform.Rotate, StyleValue.of(90))

      assert.isTrue(StyleTransformSpec.validate(spec))
    })
  })
  describe('rotate()', () => {
    it('should return rotate transform', () => {
      const spec = StyleTransformSpec.rotate(StyleValue.of(90))

      assert.equal(spec.transform, StyleTransform.Rotate)
      assert.isTrue(StyleTransformSpec.validate(spec))
    })
  })
  describe('translate()', () => {
    it('should return translate transform', () => {
      const spec = StyleTransformSpec.translate(StyleValue.of(1), StyleValue.of(2))

      assert.equal(spec.transform, StyleTransform.Translate)
      assert.isTrue(StyleTransformSpec.validate(spec))
    })
  })
  describe('scale()', () => {
    it('should return scale transform', () => {
      const spec = StyleTransformSpec.scale(StyleValue.of(1), StyleValue.of(2))

      assert.equal(spec.transform, StyleTransform.Scale)
      assert.isTrue(StyleTransformSpec.validate(spec))
    })
  })
  describe('identity()', () => {
    it('should return identity transform', () => {
      const spec = StyleTransformSpec.identity()

      assert.equal(spec.transform, StyleTransform.Identity)
      assert.isTrue(StyleTransformSpec.validate(spec))
    })
  })
})
