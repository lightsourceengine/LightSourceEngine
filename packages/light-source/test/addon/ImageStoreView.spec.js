/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ImageStoreView } from '../../src/addon'
import { assert } from 'chai'
import { afterSceneTest, beforeSceneTest } from '../index'

describe('ImageStoreView', () => {
  let scene
  beforeEach(() => {
    scene = beforeSceneTest()
  })
  afterEach(afterSceneTest)
  describe('constructor()', () => {
    it('should create an ImageStoreView object', () => {
      const imageStoreView = new ImageStoreView(scene)

      assert.lengthOf(imageStoreView.list(), 0)
    })
    it('should throw error if scene not passed into constructor', () => {
      assert.throws(() => new ImageStoreView())
    })
  })
  describe('extensions', () => {
    it('should return default image search extensions', () => {
      const imageStoreView = new ImageStoreView(scene)

      assert.sameMembers(imageStoreView.extensions, [".jpg", ".jpeg", ".png", ".gif", ".svg"])
    })
  })
})
