/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { StyleUnit } from '../../src/addon/index.js'
import { StyleValue } from '../../src/style/StyleValue.js'

const { assert } = chai

// StyleValue defined in JSStyleValue.cc
describe('StyleValue', () => {
  describe('constructor', () => {
    it('should construct a point StyleValue', () => {
      checkStyleValue(new StyleValue(3, StyleUnit.Point), 'Point', 3)
    })
    it('should construct an undefined StyleValue', () => {
      checkStyleValue(new StyleValue(), 'Undefined', 0)
    })
  })
  describe('of()', () => {
    it('should return StyleValue for pixel input', () => {
      for (const input of [2, '2px', '2PX']) {
        checkStyleValue(StyleValue.of(input), 'Point', 2)
      }
    })
    it('should return StyleValue for negative pixel input', () => {
      for (const input of [-2, '-2px', '-2PX']) {
        checkStyleValue(StyleValue.of(input), 'Point', -2)
      }
    })
    it('should return StyleValue for zero pixel input', () => {
      for (const input of [0, '0px', '0PX', '-0px', '-0PX']) {
        checkStyleValue(StyleValue.of(input), 'Point', 0)
      }
    })
    it('should return StyleValue for percent input', () => {
      checkStyleValue(StyleValue.of('100%'), 'Percent', 100)
    })
    it('should return StyleValue for negative percent input', () => {
      checkStyleValue(StyleValue.of('-100%'), 'Percent', -100)
    })
    it('should return StyleValue for 0 percent input', () => {
      checkStyleValue(StyleValue.of('0%'), 'Percent', 0)
    })
    it('should return StyleValue for vw input', () => {
      checkStyleValue(StyleValue.of('100vw'), 'ViewportWidth', 100)
    })
    it('should return StyleValue for negative vw input', () => {
      checkStyleValue(StyleValue.of('-100vw'), 'ViewportWidth', -100)
    })
    it('should return StyleValue for 0 vw input', () => {
      checkStyleValue(StyleValue.of('0vw'), 'ViewportWidth', 0)
    })
    it('should return StyleValue for vh input', () => {
      checkStyleValue(StyleValue.of('100vh'), 'ViewportHeight', 100)
    })
    it('should return StyleValue for negative vh input', () => {
      checkStyleValue(StyleValue.of('-100vh'), 'ViewportHeight', -100)
    })
    it('should return StyleValue for 0 vh input', () => {
      checkStyleValue(StyleValue.of('0vh'), 'ViewportHeight', 0)
    })
    it('should return StyleValue for vmin input', () => {
      checkStyleValue(StyleValue.of('100vmin'), 'ViewportMin', 100)
    })
    it('should return StyleValue for negative vmin input', () => {
      checkStyleValue(StyleValue.of('-100vmin'), 'ViewportMin', -100)
    })
    it('should return StyleValue for 0 vmin input', () => {
      checkStyleValue(StyleValue.of('0vmin'), 'ViewportMin', 0)
    })
    it('should return StyleValue for vmax input', () => {
      checkStyleValue(StyleValue.of('100vmax'), 'ViewportMax', 100)
    })
    it('should return StyleValue for negative vmax input', () => {
      checkStyleValue(StyleValue.of('-100vmax'), 'ViewportMax', -100)
    })
    it('should return StyleValue for 0 vmax input', () => {
      checkStyleValue(StyleValue.of('0vmax'), 'ViewportMax', 0)
    })
    it('should return StyleValue for auto input', () => {
      checkStyleValue(StyleValue.of('auto'), 'Auto', 0)
    })
    it('should return StyleValue for rem input', () => {
      checkStyleValue(StyleValue.of('1rem'), 'RootEm', 1)
    })
    it('should return StyleValue for negative rem input', () => {
      checkStyleValue(StyleValue.of('-1rem'), 'RootEm', -1)
    })
    it('should return StyleValue for 0 rem input', () => {
      checkStyleValue(StyleValue.of('0rem'), 'RootEm', 0)
    })
    it('should return StyleValue for rad input', () => {
      checkStyleValue(StyleValue.of('1rad'), 'Radian', 1)
    })
    it('should return StyleValue for negative rad input', () => {
      checkStyleValue(StyleValue.of('-1rad'), 'Radian', -1)
    })
    it('should return StyleValue for 0 rad input', () => {
      checkStyleValue(StyleValue.of('0rad'), 'Radian', 0)
    })
    it('should return StyleValue for grad input', () => {
      checkStyleValue(StyleValue.of('1grad'), 'Gradian', 1)
    })
    it('should return StyleValue for negative grad input', () => {
      checkStyleValue(StyleValue.of('-1grad'), 'Gradian', -1)
    })
    it('should return StyleValue for 0 grad input', () => {
      checkStyleValue(StyleValue.of('0grad'), 'Gradian', 0)
    })
    it('should return StyleValue for deg input', () => {
      checkStyleValue(StyleValue.of('1deg'), 'Degree', 1)
    })
    it('should return StyleValue for negative deg input', () => {
      checkStyleValue(StyleValue.of('-1deg'), 'Degree', -1)
    })
    it('should return StyleValue for 0 deg input', () => {
      checkStyleValue(StyleValue.of('0deg'), 'Degree', 0)
    })
    it('should return undefined StyleValue for no args', () => {
      assert.isTrue(StyleValue.of().isUndefined())
    })
    it('should return undefined StyleValue for invalid input', () => {
      for (const input of [undefined, null, '', '100xx', {}, [], NaN]) {
        assert.isTrue(StyleValue.of(input).isUndefined())
      }
    })
  })
})

const checkStyleValue = (styleValue, expectedType, expectedValue) => {
  assert.equal(styleValue.value, expectedValue)
  assert.equal(styleValue.unit, StyleUnit[expectedType])
  assert.isTrue(styleValue['is' + expectedType]())
  assert.isFrozen(styleValue)
}
