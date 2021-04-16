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
import { StyleUnit, StyleTransform } from '../../src/addon/index.mjs'
import {
  translate,
  scale,
  rotate,
  scaleX,
  scaleY,
  translateX,
  translateY
} from '../../src/style/transform.mjs'
import { StyleTransformSpec } from '../../src/style/StyleTransformSpec.mjs'

const { assert } = chai

describe('transform', () => {
  describe('translate()', () => {
    it('should return a StyleTransformSpec of transform type Translate', () => {
      testTransform(translate(5, 10),
        StyleTransform.TRANSLATE, 5, StyleUnit.POINT, 10, StyleUnit.POINT)
      testTransform(translate(-5, -10),
        StyleTransform.TRANSLATE, -5, StyleUnit.POINT, -10, StyleUnit.POINT)
      testTransform(translate('5px', '10px'),
        StyleTransform.TRANSLATE, 5, StyleUnit.POINT, 10, StyleUnit.POINT)
      testTransform(translate('-5px', '-10px'),
        StyleTransform.TRANSLATE, -5, StyleUnit.POINT, -10, StyleUnit.POINT)
      testTransform(translate('50%', '100%'),
        StyleTransform.TRANSLATE, 50, StyleUnit.PERCENT, 100, StyleUnit.PERCENT)
      testTransform(translate('-50%', '-100%'),
        StyleTransform.TRANSLATE, -50, StyleUnit.PERCENT, -100, StyleUnit.PERCENT)
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
        StyleTransform.TRANSLATE, 5, StyleUnit.POINT, 0, StyleUnit.POINT)
    })
    it('should return undefined for invalid x value', () => {
      assert.isUndefined(translateX(false))
    })
  })
  describe('translateY()', () => {
    it('should return a StyleTransformSpec of transform type Translate', () => {
      testTransform(translateY(5),
        StyleTransform.TRANSLATE, 0, StyleUnit.POINT, 5, StyleUnit.POINT)
    })
    it('should return undefined for invalid y value', () => {
      assert.isUndefined(translateY(false))
    })
  })
  describe('scale()', () => {
    it('should return a StyleTransformSpec of transform type Scale', () => {
      testTransform(scale(5, 10),
        StyleTransform.SCALE, 5, StyleUnit.POINT, 10, StyleUnit.POINT)
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
        StyleTransform.SCALE, 5, StyleUnit.POINT, 1, StyleUnit.POINT)
    })
    it('should return undefined for invalid x scale value', () => {
      assert.isUndefined(scaleX(false))
    })
  })
  describe('scaleX()', () => {
    it('should return a StyleTransformSpec of transform type Scale', () => {
      testTransform(scaleY(5),
        StyleTransform.SCALE, 1, StyleUnit.POINT, 5, StyleUnit.POINT)
    })
    it('should return undefined for invalid y scale value', () => {
      assert.isUndefined(scaleY(false))
    })
  })
  describe('rotate()', () => {
    it('should return a StyleTransformSpec of transform type Rotate', () => {
      testTransform(rotate(1), StyleTransform.ROTATE, 1, StyleUnit.POINT)
      testTransform(rotate('1rad'), StyleTransform.ROTATE, 1, StyleUnit.RADIAN)
      testTransform(rotate('2grad'), StyleTransform.ROTATE, 2, StyleUnit.GRADIAN)
      testTransform(rotate('90deg'), StyleTransform.ROTATE, 90, StyleUnit.DEGREE)
      testTransform(rotate('2turn'), StyleTransform.ROTATE, 2, StyleUnit.TURN)
    })
    it('should return undefined for invalid angle value', () => {
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
