/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { StyleUnit, StyleTransform } from '../../src/addon/index.js'
import {
  translate,
  scale,
  rotate,
  scaleX,
  scaleY,
  translateX,
  translateY
} from '../../src/style/transform.js'
import { StyleTransformSpec } from '../../src/style/StyleTransformSpec.js'

const { assert } = chai

describe('transform', () => {
  describe('translate()', () => {
    it('should return a StyleTransformSpec of transform type Translate', () => {
      testTransform(translate(5, 10),
        StyleTransform.Translate, 5, StyleUnit.Point, 10, StyleUnit.Point)
      testTransform(translate(-5, -10),
        StyleTransform.Translate, -5, StyleUnit.Point, -10, StyleUnit.Point)
      testTransform(translate('5px', '10px'),
        StyleTransform.Translate, 5, StyleUnit.Point, 10, StyleUnit.Point)
      testTransform(translate('-5px', '-10px'),
        StyleTransform.Translate, -5, StyleUnit.Point, -10, StyleUnit.Point)
      testTransform(translate('50%', '100%'),
        StyleTransform.Translate, 50, StyleUnit.Percent, 100, StyleUnit.Percent)
      testTransform(translate('-50%', '-100%'),
        StyleTransform.Translate, -50, StyleUnit.Percent, -100, StyleUnit.Percent)
    })
    it('should return undefined for invalid x and y values', () => {
      assert.isUndefined(translate(false, false))
      assert.isUndefined(translate('100xx', 10))
      assert.isUndefined(translate(100, ''))
    })
  })
  describe('translateX()', () => {
    it('should return a StyleTransformSpec of transform type Translate', () => {
      testTransform(translateX(5),
        StyleTransform.Translate, 5, StyleUnit.Point, 0, StyleUnit.Point)
    })
    it('should return undefined for invalid x value', () => {
      assert.isUndefined(translateX(false))
    })
  })
  describe('translateY()', () => {
    it('should return a StyleTransformSpec of transform type Translate', () => {
      testTransform(translateY(5),
        StyleTransform.Translate, 0, StyleUnit.Point, 5, StyleUnit.Point)
    })
    it('should return undefined for invalid y value', () => {
      assert.isUndefined(translateY(false))
    })
  })
  describe('scale()', () => {
    it('should return a StyleTransformSpec of transform type Scale', () => {
      testTransform(scale(5, 10),
        StyleTransform.Scale, 5, StyleUnit.Point, 10, StyleUnit.Point)
    })
    it('should return undefined for invalid scale values', () => {
      assert.isUndefined(scale(false, false))
      assert.isUndefined(scale('100deg', 10))
      assert.isUndefined(scale('100%', 10))
    })
  })
  describe('scaleX()', () => {
    it('should return a StyleTransformSpec of transform type Scale', () => {
      testTransform(scaleX(5),
        StyleTransform.Scale, 5, StyleUnit.Point, 1, StyleUnit.Point)
    })
    it('should return undefined for invalid x scale value', () => {
      assert.isUndefined(scaleX(false))
    })
  })
  describe('scaleX()', () => {
    it('should return a StyleTransformSpec of transform type Scale', () => {
      testTransform(scaleY(5),
        StyleTransform.Scale, 1, StyleUnit.Point, 5, StyleUnit.Point)
    })
    it('should return undefined for invalid y scale value', () => {
      assert.isUndefined(scaleY(false))
    })
  })
  describe('rotate()', () => {
    it('should return a StyleTransformSpec of transform type Rotate', () => {
      testTransform(rotate(1), StyleTransform.Rotate, 1, StyleUnit.Point)
      testTransform(rotate('1rad'), StyleTransform.Rotate, 1, StyleUnit.Radian)
      testTransform(rotate('2grad'), StyleTransform.Rotate, 2, StyleUnit.Gradian)
      testTransform(rotate('90deg'), StyleTransform.Rotate, 90, StyleUnit.Degree)
      testTransform(rotate('2turn'), StyleTransform.Rotate, 2, StyleUnit.Turn)
    })
    it('should reutrn undefined for invalid angle value', () => {
      assert.isUndefined(rotate(false))
      assert.isUndefined(rotate('100vw'))
      assert.isUndefined(rotate({}))
    })
  })
})

const testTransform = (result, ...expected) => {
  assert.instanceOf(result, StyleTransformSpec)
  assert.equal(result.transform, expected[0])

  if (expected.length === 3) {
    testStyleValue(result.angle, expected[1], expected[2])
  } else if (expected.length === 5) {
    testStyleValue(result.x, expected[1], expected[2])
    testStyleValue(result.y, expected[3], expected[4])
  } else {
    assert.fail('expected arg list is unreadable')
  }
}

const testStyleValue = (value, expectedValue, expectedUnit) => {
  assert.equal(value.value, expectedValue)
  assert.equal(value.unit, expectedUnit)
}
