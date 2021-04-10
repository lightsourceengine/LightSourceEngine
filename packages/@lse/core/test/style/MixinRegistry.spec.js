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
import { MixinRegistry, resetMixinRegistry } from '../../src/style/MixinRegistry.js'

const { assert } = chai
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
