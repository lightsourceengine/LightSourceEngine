/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ImageStoreView } from '../../src/addon'
import { assert } from 'chai'
import { afterSceneTest, beforeSceneTest } from '../index'

const testImage = 'test/resources/640x480.png'

describe('ImageStoreView', () => {
  let scene
  let imageStoreView
  beforeEach(() => {
    scene = beforeSceneTest()
    imageStoreView = new ImageStoreView(scene)
  })
  afterEach(() => {
    imageStoreView = null
    afterSceneTest()
  })
  describe('constructor()', () => {
    it('should create an ImageStoreView object', () => {
      assert.lengthOf(imageStoreView.list(), 0)
    })
    it('should throw error if scene not passed into constructor', () => {
      assert.throws(() => new ImageStoreView())
    })
  })
  describe('extensions', () => {
    it('should return default image search extensions', () => {
      assert.sameMembers(imageStoreView.extensions, [".jpg", ".jpeg", ".png", ".gif", ".svg"])
    })
  })
  describe('add()', () => {
    it('should add image with string path', () => {
      imageStoreView.add(testImage)
      assert.include(imageStoreView.list()[0], { id: testImage, uri: testImage })
    })
    it('should add image with uri', () => {
      imageStoreView.add({ uri: testImage })
      assert.include(imageStoreView.list()[0], { id: testImage, uri: testImage })
    })
    it('should add image with custom id', () => {
      const id = 'test'

      imageStoreView.add({ id, uri: testImage })
      assert.include(imageStoreView.list()[0], { id, uri: testImage })
    })
    it('should add image with capInsets', () => {
      const capInsets = { top: 1, right: 2, bottom: 3, left: 4 }
      const id = 'test'

      imageStoreView.add({ id, uri: testImage, capInsets })
      assert.deepInclude(imageStoreView.list()[0], { id, uri: testImage, capInsets })
    })
    it('should throw when no arguments passed', () => {
      assert.throws(() => imageStoreView.add())
    })
    it('should throw when uri is missing', () => {
      assert.throws(() => imageStoreView.add({ id: 'test' }))
    })
    it('should throw when uri is not a string', () => {
      assert.throws(() => imageStoreView.add({ uri: null }))
      assert.throws(() => imageStoreView.add({ uri: 3 }))
    })
  })
})
