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
    stage.fps = 0
    stage = null
  })
  describe('fps', () => {
    it('should set fps', () => {
      stage.fps = 30
      assert.equal(stage.fps, 30)
    })
    it('should reset to default 60 when set to 0', () => {
      stage.fps = 30
      stage.fps = 0
      assert.equal(stage.fps, 60)
    })
    it('should throw error for invalid fps value', () => {
      for (const input of [-1, NaN, '', {}, [], 100]) {
        assert.throws(() => { stage.fps = input })
      }
    })
  })
})
