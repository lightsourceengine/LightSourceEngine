/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Stage } from '../../src/stage/Stage'

let stage

describe('Stage', () => {
  beforeEach(() => {
    stage = new Stage()
    stage.init({ adapter: 'light-source-ref', audioAdapter: 'light-source-ref' })
  })
  afterEach(() => {
    stage[Symbol.for('destroy')]()
    stage = null
  })
  describe('constructor', () => {
    it('should create a Stage object', () => {
      const testStage = new Stage()

      assert.isOk(testStage.input)
      assert.isOk(testStage.audio)
      assert.isOk(testStage.font)
      assert.lengthOf(testStage.displays, 0)
      assert.equal(testStage.resourcePath, '')
      assert.equal(stage.fps, 60)
    })
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
