/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { test } from '../test-env.js'
import { PluginType } from '../../src/addon/PluginType.js'

const { assert } = chai
let stage

describe('Stage', () => {
  beforeEach(() => {
    stage = test.stage
  })
  afterEach(() => {
    stage.resetFrameRate()
    stage = null
  })
  describe('setFrameRate()', () => {
    it('should set to 30', () => {
      stage.setFrameRate(30)
      assert.equal(stage.getFrameRate(), 30)
    })
    it('should throw error for invalid fps value', () => {
      for (const input of [-1, 0, NaN, '', {}, []]) {
        assert.throws(() => { stage.setFrameRate(input) })
      }
    })
  })
  describe('resetFrameRate()', () => {
    it('should reset fps to 60', () => {
      stage.setFrameRate(30)
      assert.equal(stage.getFrameRate(), 30)
      stage.resetFrameRate()
      assert.equal(stage.getFrameRate(), 60)
    })
  })
  describe('isRunning()', () => {
    it('should return true for test stage instance', () => {
      assert.isTrue(stage.isRunning())
    })
  })
  describe('isAttached()', () => {
    it('should return true for test stage instance', () => {
      assert.isTrue(stage.isAttached())
    })
  })
  describe('isConfigured()', () => {
    it('should return true for test stage instance', () => {
      assert.isTrue(stage.isConfigured())
    })
  })
  describe('wasQuitRequested()', () => {
    it('should return false for test stage instance', () => {
      assert.isFalse(stage.wasQuitRequested())
    })
  })
  describe('hasPluginType()', () => {
    it('should have platform plugin for test stage instance', () => {
      assert.isTrue(stage.hasPluginType(PluginType.PLATFORM))
    })
    it('should have audio plugin for test stage instance', () => {
      assert.isTrue(stage.hasPluginType(PluginType.AUDIO))
    })
  })
})
