/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { StyleUnit, StyleValue } from '../../src/addon'

// StyleValue defined in JSStyleValue.cc
describe('StyleValue', () => {
  describe('constructor', () => {
    it('should construct a point StyleValue', () => {
      const styleValue = new StyleValue(3, StyleUnit.Point)

      assert.equal(styleValue.unit, StyleUnit.Point)
      assert.equal(styleValue.value, 3)
      assert.isFalse(styleValue.isUndefined())
      assert.isFrozen(styleValue)
    })
    it('should construct an undefined StyleValue', () => {
      const styleValue = new StyleValue()

      assert.equal(styleValue.unit, StyleUnit.Undefined)
      assert.equal(styleValue.value, 0)
      assert.isTrue(styleValue.isUndefined())
      assert.isFrozen(styleValue)
    })
  })
  describe('of()', () => {
    it('should return StyleValue for pixel input', () => {
      for (const input of [2, '2px']) {
        const styleValue = StyleValue.of(input)

        assert.equal(styleValue.unit, StyleUnit.Point)
        assert.equal(styleValue.value, 2)
        assert.isFalse(styleValue.isUndefined())
        assert.isFrozen(styleValue)
      }
    })
    it('should return undefined StyleValue for invalid input', () => {
      for (const input of [undefined, null, '', '100xx', {}, [], NaN]) {
        assert.isTrue(StyleValue.of(input).isUndefined())
      }
    })
  })
})
