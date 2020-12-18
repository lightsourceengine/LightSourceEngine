/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { ShorthandRegistry, resetShorthandRegistry } from '../../src/style/ShorthandRegistry.js'

const { assert } = chai
const testId = '@test'
const sizeId = '@size'
const unknownId = '@unknown'

describe('ShorthandRegistry', () => {
  afterEach(() => resetShorthandRegistry())
  describe('add()', () => {
    it('should add new shorthand', () => {
      ShorthandRegistry.add(testId, (style, value) => {})
      assert.isTrue(ShorthandRegistry.has(testId))
      assert.isFunction(ShorthandRegistry.get(testId))
    })
    it('should throw Error if id is not a string', () => {
      for (const input of [null, undefined, '', NaN, 3, {}, []]) {
        assert.throws(() => ShorthandRegistry.add(input, {}))
      }
    })
    it('should throw Error if id does not have @ prefix', () => {
      assert.throws(() => ShorthandRegistry.add('test', {}))
    })
    it('should throw Error if id is reserved', () => {
      assert.throws(() => ShorthandRegistry.add('@expand', {}))
    })
    it('should throw Error if style is not a function', () => {
      for (const input of [null, undefined, '', NaN, 3, {}, []]) {
        assert.throws(() => ShorthandRegistry.add(testId, input))
      }
    })
    it('should throw Error if id already exists', () => {
      ShorthandRegistry.add(testId, (style, value) => {})
      assert.isTrue(ShorthandRegistry.has(testId))
      assert.throws(() => ShorthandRegistry.add(testId, (style, value) => {}))
    })
  })
  describe('get()', () => {
    it('should return @size expand function', () => {
      assert.isFunction(ShorthandRegistry.get(sizeId))
    })
    it('should return undefined if id does not exist', () => {
      assert.isUndefined(ShorthandRegistry.get(unknownId))
    })
  })
  describe('has()', () => {
    it('should return true for @size', () => {
      assert.isTrue(ShorthandRegistry.has(sizeId))
    })
    it('should return false for unknown Shorthand', () => {
      assert.isFalse(ShorthandRegistry.has(unknownId))
    })
  })
  describe('delete()', () => {
    it('should remove @size', () => {
      assert.isTrue(ShorthandRegistry.has(sizeId))
      ShorthandRegistry.delete(sizeId)
      assert.isFalse(ShorthandRegistry.has(sizeId))
    })
    it('should do nothing if id does not exist', () => {
      ShorthandRegistry.delete(unknownId)
    })
  })
  describe('builtins', () => {
    it('should expand @trbl', () => {
      testExpand('@trbl', 3, { top: 3, right: 3, bottom: 3, left: 3 },
        [1, 2, 3, 4], { top: 1, right: 2, bottom: 3, left: 4 })
    })
    it('should expand @wh', () => {
      testExpand('@wh', 3, { width: 3, height: 3 },
        [3, 4], { width: 3, height: 4 })
    })
    it('should expand @size', () => {
      testExpand('@size', 3, { width: 3, height: 3 },
        [3, 4], { width: 3, height: 4 })
    })
    it('should expand @objectPosition', () => {
      testExpand('@objectPosition', 3, { objectPositionX: 3, objectPositionY: 3 },
        [3, 4], { objectPositionX: 3, objectPositionY: 4 })
    })
    it('should expand @backgroundPosition', () => {
      testExpand('@backgroundPosition', 3, { backgroundPositionX: 3, backgroundPositionY: 3 },
        [3, 4], { backgroundPositionX: 3, backgroundPositionY: 4 })
    })
    it('should expand @transformOrigin', () => {
      testExpand('@transformOrigin', 3, { transformOriginX: 3, transformOriginY: 3 },
        [3, 4], { transformOriginX: 3, transformOriginY: 4 })
    })
  })
})

const testExpand = (id, input1, expected1, input2, expected2) => {
  const expand = ShorthandRegistry.get(id)
  let style = {}

  expand(style, input1)
  assert.deepInclude(style, expected1)

  style = {}

  expand(style, input2)
  assert.deepInclude(style, expected2)
}
