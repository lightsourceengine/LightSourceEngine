/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { MixinRegistry, resetMixinRegistry } from '../../src/style/MixinRegistry'

const testId = '%test'
const absoluteFillId = '%absoluteFill'
const unknownId = '%unknown'

describe('MixinRegistry', () => {
  afterEach(() => resetMixinRegistry())
  describe('add()', () => {
    it('should add new mixin', () => {
      MixinRegistry.add(testId, { border: 3 })
      assert.isTrue(MixinRegistry.has(testId))
      assert.equal(MixinRegistry.get(testId).border, 3)
    })
    it('should throw Error if id is not a string', () => {
      for (const input of [null, undefined, '', NaN, 3, {}, []]) {
        assert.throws(() => MixinRegistry.add(input, {}))
      }
    })
    it('should throw Error if id does not have % prefix', () => {
      assert.throws(() => MixinRegistry.add('test', {}))
    })
    it('should throw Error if style is not an object', () => {
      for (const input of [null, undefined, '', NaN, 3]) {
        assert.throws(() => MixinRegistry.add(testId, input))
      }
    })
    it('should throw Error if id already exists', () => {
      MixinRegistry.add(testId, {})
      assert.isTrue(MixinRegistry.has(testId))
      assert.throws(() => MixinRegistry.add(testId, {}))
    })
  })
  describe('get()', () => {
    it('should return %absoluteFill', () => {
      assert.equal(MixinRegistry.get(absoluteFillId).position, 'absolute')
    })
    it('should return undefined if mixin does not exist', () => {
      assert.isUndefined(MixinRegistry.get(unknownId))
    })
  })
  describe('has()', () => {
    it('should return true for %absoluteFill', () => {
      assert.isTrue(MixinRegistry.has(absoluteFillId))
    })
    it('should return false for unknown mixin', () => {
      assert.isFalse(MixinRegistry.has(unknownId))
    })
  })
  describe('delete()', () => {
    it('should remove %absoluteFill', () => {
      assert.isTrue(MixinRegistry.has(absoluteFillId))
      MixinRegistry.delete(absoluteFillId)
      assert.isFalse(MixinRegistry.has(absoluteFillId))
    })
    it('should do nothing if id does not exist', () => {
      MixinRegistry.delete(unknownId)
    })
  })
})
