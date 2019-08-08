/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest, createNode } from './index'
import { ImageSceneNode } from '../src/addon-light-source'
import { assert } from 'chai'

describe('ImageSceneNode', () => {
  before(beforeSceneTest)
  after(afterSceneTest)
  describe('constructor()', () => {
    it('should throw Error when arg is not a Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new ImageSceneNode(input))
      }
    })
  })
  describe('appendChild()', () => {
    it('should always throw Error', () => {
      assert.throws(() => createNode('img').appendChild(createNode('img')))
    })
  })
  describe('removeChild()', () => {
    it('should always throw Error', () => {
      assert.throws(() => createNode('img').removeChild(createNode('img')))
    })
  })
  describe('insertBefore()', () => {
    it('should always throw Error', () => {
      assert.throws(() => createNode('img').insertBefore(createNode('img'), createNode('img')))
    })
  })
  describe('src', () => {
    it('should be assignable to a uri string', () => {
      const node = createNode('img')

      node.src = 'image.jpg'
      assert.include(node.src, { id: 'image.jpg', uri: 'image.jpg' })
    })
    it('should be null when assigned a non-string value', () => {
      const node = createNode('img')

      for (const input of ['', null, undefined, 3]) {
        node.src = input
        assert.isNull(node.src)
      }
    })
    xit('should be throw Error when assigned an invalid Object', () => {
      const node = createNode('img')

      for (const input of [{}, { id: 'image.jpg' }]) {
        assert.throws(() => { node.src = input })
      }
    })
  })
})
