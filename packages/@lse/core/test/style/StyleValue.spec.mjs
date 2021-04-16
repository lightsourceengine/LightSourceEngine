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
import { StyleUnit } from '../../src/addon/index.mjs'
import { StyleValue } from '../../src/style/StyleValue.mjs'

const { assert } = chai

describe('StyleValue', () => {
  describe('constructor', () => {
    it('should construct a point StyleValue', () => {
      checkStyleValue(new StyleValue(3, StyleUnit.POINT), 'POINT', 3)
    })
    it('should construct an undefined StyleValue', () => {
      checkStyleValue(new StyleValue(), 'UNDEFINED', 0)
    })
  })
  describe('of()', () => {
    it('should return StyleValue for pixel input', () => {
      for (const input of [2, '2px', '2PX']) {
        checkStyleValue(StyleValue.of(input), 'POINT', 2)
      }
    })
    it('should return StyleValue for negative pixel input', () => {
      for (const input of [-2, '-2px', '-2PX']) {
        checkStyleValue(StyleValue.of(input), 'POINT', -2)
      }
    })
    it('should return StyleValue for zero pixel input', () => {
      for (const input of [0, '0px', '0PX', '-0px', '-0PX']) {
        checkStyleValue(StyleValue.of(input), 'POINT', 0)
      }
    })
    it('should return StyleValue for percent input', () => {
      checkStyleValue(StyleValue.of('100%'), 'PERCENT', 100)
    })
    it('should return StyleValue for negative percent input', () => {
      checkStyleValue(StyleValue.of('-100%'), 'PERCENT', -100)
    })
    it('should return StyleValue for 0 percent input', () => {
      checkStyleValue(StyleValue.of('0%'), 'PERCENT', 0)
    })
    it('should return StyleValue for vw input', () => {
      checkStyleValue(StyleValue.of('100vw'), 'VIEWPORT_WIDTH', 100)
    })
    it('should return StyleValue for negative vw input', () => {
      checkStyleValue(StyleValue.of('-100vw'), 'VIEWPORT_WIDTH', -100)
    })
    it('should return StyleValue for 0 vw input', () => {
      checkStyleValue(StyleValue.of('0vw'), 'VIEWPORT_WIDTH', 0)
    })
    it('should return StyleValue for vh input', () => {
      checkStyleValue(StyleValue.of('100vh'), 'VIEWPORT_HEIGHT', 100)
    })
    it('should return StyleValue for negative vh input', () => {
      checkStyleValue(StyleValue.of('-100vh'), 'VIEWPORT_HEIGHT', -100)
    })
    it('should return StyleValue for 0 vh input', () => {
      checkStyleValue(StyleValue.of('0vh'), 'VIEWPORT_HEIGHT', 0)
    })
    it('should return StyleValue for vmin input', () => {
      checkStyleValue(StyleValue.of('100vmin'), 'VIEWPORT_MIN', 100)
    })
    it('should return StyleValue for negative vmin input', () => {
      checkStyleValue(StyleValue.of('-100vmin'), 'VIEWPORT_MIN', -100)
    })
    it('should return StyleValue for 0 vmin input', () => {
      checkStyleValue(StyleValue.of('0vmin'), 'VIEWPORT_MIN', 0)
    })
    it('should return StyleValue for vmax input', () => {
      checkStyleValue(StyleValue.of('100vmax'), 'VIEWPORT_MAX', 100)
    })
    it('should return StyleValue for negative vmax input', () => {
      checkStyleValue(StyleValue.of('-100vmax'), 'VIEWPORT_MAX', -100)
    })
    it('should return StyleValue for 0 vmax input', () => {
      checkStyleValue(StyleValue.of('0vmax'), 'VIEWPORT_MAX', 0)
    })
    it('should return StyleValue for auto input', () => {
      checkStyleValue(StyleValue.of('auto'), 'AUTO', 0)
    })
    it('should return StyleValue for rem input', () => {
      checkStyleValue(StyleValue.of('1rem'), 'REM', 1)
    })
    it('should return StyleValue for negative rem input', () => {
      checkStyleValue(StyleValue.of('-1rem'), 'REM', -1)
    })
    it('should return StyleValue for 0 rem input', () => {
      checkStyleValue(StyleValue.of('0rem'), 'REM', 0)
    })
    it('should return StyleValue for rad input', () => {
      checkStyleValue(StyleValue.of('1rad'), 'RADIAN', 1)
    })
    it('should return StyleValue for negative rad input', () => {
      checkStyleValue(StyleValue.of('-1rad'), 'RADIAN', -1)
    })
    it('should return StyleValue for 0 rad input', () => {
      checkStyleValue(StyleValue.of('0rad'), 'RADIAN', 0)
    })
    it('should return StyleValue for grad input', () => {
      checkStyleValue(StyleValue.of('1grad'), 'GRADIAN', 1)
    })
    it('should return StyleValue for negative grad input', () => {
      checkStyleValue(StyleValue.of('-1grad'), 'GRADIAN', -1)
    })
    it('should return StyleValue for 0 grad input', () => {
      checkStyleValue(StyleValue.of('0grad'), 'GRADIAN', 0)
    })
    it('should return StyleValue for deg input', () => {
      checkStyleValue(StyleValue.of('1deg'), 'DEGREE', 1)
    })
    it('should return StyleValue for negative deg input', () => {
      checkStyleValue(StyleValue.of('-1deg'), 'DEGREE', -1)
    })
    it('should return StyleValue for 0 deg input', () => {
      checkStyleValue(StyleValue.of('0deg'), 'DEGREE', 0)
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
  assert.isFrozen(styleValue)
}
