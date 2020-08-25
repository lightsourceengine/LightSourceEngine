/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { test } from '../test-env'

let stage

describe('Stage', () => {
  beforeEach(() => {
    stage = test.stage
  })
  afterEach(() => {
    stage.setFrameRate(0)
    stage = null
  })
  describe('frameRate', () => {
    it('should set to 30', () => {
      stage.setFrameRate(30)
      assert.equal(stage.getFrameRate(), 30)
    })
    it('should reset to default 60 when set to 0', () => {
      stage.setFrameRate(30)
      assert.equal(stage.getFrameRate(), 30)
      stage.setFrameRate(0)
      assert.equal(stage.getFrameRate(), 60)
    })
    it('should throw error for invalid fps value', () => {
      for (const input of [-1, NaN, '', {}, [], 100]) {
        assert.throws(() => { stage.setFrameRate(input) })
      }
    })
  })
})
