/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { StyleUnit, StyleTransform, StyleTransformSpec } from '../../src/addon/index.js'
import {
  translate,
  scale,
  rotate,
  scaleX,
  scaleY,
  translateX,
  translateY,
  isRotate,
  isScale,
  isTranslate,
  getRotateAngle,
  getTranslateX,
  getTranslateY,
  getScaleX,
  getScaleY
} from '../../src/style/transform.js'

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
  describe('isRotate()', () => {
    it('should return true for rotate transform', () => {
      assert.isTrue(isRotate(rotate(10)))
    })
    it('should return false for non-rotate transform', () => {
      assert.isFalse(isRotate(translate(10, 10)))
      assert.isFalse(isRotate(100))
    })
  })
  describe('isScale()', () => {
    it('should return true for scale transform', () => {
      assert.isTrue(isScale(scale(10, 10)))
      assert.isTrue(isScale(scaleX(10)))
      assert.isTrue(isScale(scaleY(10)))
    })
    it('should return false for non-scale transform', () => {
      assert.isFalse(isScale(translate(10, 10)))
      assert.isFalse(isScale(100))
    })
  })
  describe('isTranslate()', () => {
    it('should return true for translate transform', () => {
      assert.isTrue(isTranslate(translate(10, 10)))
      assert.isTrue(isTranslate(translateX(10)))
      assert.isTrue(isTranslate(translateY(10)))
    })
    it('should return false for non-translate transform', () => {
      assert.isFalse(isTranslate(scale(10, 10)))
      assert.isFalse(isTranslate(100))
    })
  })
  describe('getRotateAngle()', () => {
    it('should return a StyleValue containing the rotation angle value and unit', () => {
      testStyleValue(getRotateAngle(rotate('90deg')), 90, StyleUnit.Degree)
      testStyleValue(getRotateAngle(rotate('2rad')), 2, StyleUnit.Radian)
      testStyleValue(getRotateAngle(rotate('2grad')), 2, StyleUnit.Gradian)
      testStyleValue(getRotateAngle(rotate('1turn')), 1, StyleUnit.Turn)
      testStyleValue(getRotateAngle(rotate(3)), 3, StyleUnit.Point)
    })
    it('should return an empty StyleValue when passed an invalid argument', () => {
      assert.isTrue(getRotateAngle(translateX(3)).isUndefined())
      assert.isTrue(getRotateAngle(100).isUndefined())
    })
  })
  describe('getTranslateX()', () => {
    it('should return the translate x StyleValue', () => {
      testStyleValue(getTranslateX(translate(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateX(translateX(10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateX(translateY(10)), 0, StyleUnit.Point)
    })
    it('should return an empty StyleValue when passed an invalid argument', () => {
      assert.isTrue(getTranslateX(scaleX(3)).isUndefined())
      assert.isTrue(getTranslateX(100).isUndefined())
    })
  })
  describe('getTranslateY()', () => {
    it('should return the translate y StyleValue', () => {
      testStyleValue(getTranslateY(translate(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateY(translateY(10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateY(translateX(10)), 0, StyleUnit.Point)
    })
    it('should return an undefined StyleValue when passed an invalid argument', () => {
      assert.isTrue(getTranslateY(scaleX(3)).isUndefined())
      assert.isTrue(getTranslateY(100).isUndefined())
    })
  })
  describe('getScaleX()', () => {
    it('should return the scale x StyleValue', () => {
      testStyleValue(getScaleX(scale(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getScaleX(scaleX(10)), 10, StyleUnit.Point)
      testStyleValue(getScaleX(scaleY(10)), 1, StyleUnit.Point)
    })
    it('should return an undefined StyleValue when passed an invalid argument', () => {
      assert.isTrue(getScaleX(translateX(3)).isUndefined())
      assert.isTrue(getScaleX(100).isUndefined())
    })
  })
  describe('getScaleY()', () => {
    it('should return the scale y StyleValue', () => {
      testStyleValue(getScaleY(scale(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getScaleY(scaleY(10)), 10, StyleUnit.Point)
      testStyleValue(getScaleY(scaleX(10)), 1, StyleUnit.Point)
    })
    it('should return an undefined StyleValue when passed an invalid argument', () => {
      assert.isTrue(getScaleY(translateX(3)).isUndefined())
      assert.isTrue(getScaleY(100).isUndefined())
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
