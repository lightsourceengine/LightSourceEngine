/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { StyleUnit, StyleTransform } from '../../src/addon'
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
} from '../../src/style/transform'

describe('transform', () => {
  describe('translate()', () => {
    it('should Float32Array with translate data', () => {
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
    it('should Float32Array with translate data', () => {
      testTransform(translateX(5),
        StyleTransform.Translate, 5, StyleUnit.Point, 0, StyleUnit.Point)
    })
    it('should return undefined for invalid x value', () => {
      assert.isUndefined(translateX(false))
    })
  })
  describe('translateY()', () => {
    it('should Float32Array with translate data', () => {
      testTransform(translateY(5),
        StyleTransform.Translate, 0, StyleUnit.Point, 5, StyleUnit.Point)
    })
    it('should return undefined for invalid y value', () => {
      assert.isUndefined(translateY(false))
    })
  })
  describe('scale()', () => {
    it('should Float32Array with scale data', () => {
      testTransform(scale(5, 10),
        StyleTransform.Scale, 5, StyleUnit.Point, 10, StyleUnit.Point)
    })
    it('should return undefined for invalid scale values', () => {
      assert.isUndefined(scale(false, false))
      assert.isUndefined(scale('100px', 10))
      assert.isUndefined(scale('100%', 10))
    })
  })
  describe('scaleX()', () => {
    it('should Float32Array with translate data', () => {
      testTransform(scaleX(5),
        StyleTransform.Scale, 5, StyleUnit.Point, 1, StyleUnit.Point)
    })
    it('should return undefined for invalid x scale value', () => {
      assert.isUndefined(scaleX(false))
    })
  })
  describe('scaleX()', () => {
    it('should Float32Array with translate data', () => {
      testTransform(scaleY(5),
        StyleTransform.Scale, 1, StyleUnit.Point, 5, StyleUnit.Point)
    })
    it('should return undefined for invalid y scale value', () => {
      assert.isUndefined(scaleY(false))
    })
  })
  describe('rotate()', () => {
    it('should Float32Array with rotate data', () => {
      testTransform(rotate(1), StyleTransform.Rotate, 1, StyleUnit.Radian)
      testTransform(rotate('1rad'), StyleTransform.Rotate, 1, StyleUnit.Radian)
      testTransform(rotate('2grad'), StyleTransform.Rotate, 2, StyleUnit.Gradian)
      testTransform(rotate('90deg'), StyleTransform.Rotate, 90, StyleUnit.Degree)
      testTransform(rotate('2turn'), StyleTransform.Rotate, 2, StyleUnit.Turn)
    })
    it('should reutrn undefined for invalid angle value', () => {
      assert.isUndefined(rotate(false))
      assert.isUndefined(rotate('100px'))
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
    it('should return an array pair containing the rotation angle value and unit', () => {
      testStyleValue(getRotateAngle(rotate('90deg')), 90, StyleUnit.Degree)
      testStyleValue(getRotateAngle(rotate('2rad')), 2, StyleUnit.Radian)
      testStyleValue(getRotateAngle(rotate('2grad')), 2, StyleUnit.Gradian)
      testStyleValue(getRotateAngle(rotate('1turn')), 1, StyleUnit.Turn)
      testStyleValue(getRotateAngle(rotate(3)), 3, StyleUnit.Radian)
    })
    it('should return an empty array when passed an invalid argument', () => {
      assert.isEmpty(getRotateAngle(translateX(3)))
      assert.isEmpty(getRotateAngle(100))
    })
  })
  describe('getTranslateX()', () => {
    it('should return an array pair containing the x value and unit', () => {
      testStyleValue(getTranslateX(translate(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateX(translateX(10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateX(translateY(10)), 0, StyleUnit.Point)
    })
    it('should return an empty array when passed an invalid argument', () => {
      assert.isEmpty(getTranslateX(scaleX(3)))
      assert.isEmpty(getTranslateX(100))
    })
  })
  describe('getTranslateY()', () => {
    it('should return an array pair containing the y value and unit', () => {
      testStyleValue(getTranslateY(translate(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateY(translateY(10)), 10, StyleUnit.Point)
      testStyleValue(getTranslateY(translateX(10)), 0, StyleUnit.Point)
    })
    it('should return an empty array when passed an invalid argument', () => {
      assert.isEmpty(getTranslateY(scaleX(3)))
      assert.isEmpty(getTranslateY(100))
    })
  })
  describe('getScaleX()', () => {
    it('should return an array pair containing the x value and unit', () => {
      testStyleValue(getScaleX(scale(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getScaleX(scaleX(10)), 10, StyleUnit.Point)
      testStyleValue(getScaleX(scaleY(10)), 1, StyleUnit.Point)
    })
    it('should return an empty array when passed an invalid argument', () => {
      assert.isEmpty(getScaleX(translateX(3)))
      assert.isEmpty(getScaleX(100))
    })
  })
  describe('getScaleY()', () => {
    it('should return an array pair containing the y value and unit', () => {
      testStyleValue(getScaleY(scale(10, 10)), 10, StyleUnit.Point)
      testStyleValue(getScaleY(scaleY(10)), 10, StyleUnit.Point)
      testStyleValue(getScaleY(scaleX(10)), 1, StyleUnit.Point)
    })
    it('should return an empty array when passed an invalid argument', () => {
      assert.isEmpty(getScaleY(translateX(3)))
      assert.isEmpty(getScaleY(100))
    })
  })
})

const testTransform = (transform, ...expected) => {
  assert.instanceOf(transform, Float32Array)
  assert.lengthOf(transform, expected.length)

  for (let i = 0; i < expected.length; i++) {
    assert.equal(transform[i], expected[i])
  }
}

const testStyleValue = (value, expectedValue, expectedUnit) => {
  assert.lengthOf(value, 2)
  assert.equal(value[0], expectedValue)
  assert.equal(value[1], expectedUnit)
}
