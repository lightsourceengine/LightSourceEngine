/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { FontStoreView } from '../../src/addon'
import { afterSceneTest, beforeSceneTest } from '../index'

describe('FontStoreView', () => {
  let fontStore
  let stage
  beforeEach(() => {
    stage = beforeSceneTest().stage
    fontStore = new FontStoreView(stage)
  })
  afterEach(() => {
    afterSceneTest()
    stage = null
    fontStore = null
  })
  describe('constructor', () => {
    it('should throw Error when no argument passed', () => {
      assert.throws(() => new FontStoreView())
    })
    it('should throw Error when stage instance not passed', () => {
      for (const arg of [{}, null, undefined, '']) {
        assert.throws(() => new FontStoreView(arg))
      }
    })
  })
  describe('add()', () => {
    it('should add new font resource', () => {
      fontStore.add({ family: 'test', uri: 'test/resources/arrow.ttf' })

      const fonts = fontStore.all()

      assert.lengthOf(fonts, 1)
      assert.equal(fonts[0].family, 'test')
      assert.equal(fonts[0].style, 'normal')
      assert.equal(fonts[0].weight, 'normal')
      assert.equal(fonts[0].index, 0)
      assert.equal(fonts[0].uri, 'test/resources/arrow.ttf')
      assert.equal(fonts[0].state, 'loading')
    })
    it('should throw Error if font already has been added', () => {
      fontStore.add({ family: 'test', uri: 'test/resources/arrow.ttf' })
      assert.throws(() => fontStore.add({ family: 'test', uri: 'test/resources/arrow.ttf' }))
    })
  })
  describe('remove()', () => {
    it('should remove font resource', () => {
      fontStore.add({ family: 'test', uri: 'test/resources/arrow.ttf' })
      assert.lengthOf(fontStore.all(), 1)
      fontStore.remove({ family: 'test', style: 'normal', weight: 'normal' })
      assert.lengthOf(fontStore.all(), 0)
    })
    it('should throw Error for invalid font id type', () => {
      for (const input of [null, undefined, {}, 'test']) {
        assert.throws(() => fontStore.remove(input))
      }
    })
    it('should throw Error for invalid font id', () => {
      const inputs = [
        { family: 'test', style: 'normal' },
        { family: 'test', weight: 'normal' },
        { family: 'test', style: 'invalid', weight: 'normal' },
        { family: 'test', style: 'normal', weight: 'invalid' },
      ]

      for (const input of inputs) {
        assert.throws(() => fontStore.remove(input))
      }
    })
  })
  describe('all()', () => {
    it('should return empty array when no fonts present', () => {
      assert.lengthOf(fontStore.all(), 0)
    })
  })
})
