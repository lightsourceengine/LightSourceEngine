/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest, createNode } from '..'
import { assert } from 'chai'
import { TextSceneNode } from '../../src/addon'

describe('TextSceneNode', () => {
  before(beforeSceneTest)
  after(afterSceneTest)
  describe('constructor()', () => {
    it('should create uninitialized node when passed an invalid Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new TextSceneNode(input))
      }
    })
  })
  describe('appendChild()', () => {
    it('should always throw Error', () => {
      assert.throws(() => createNode('text').appendChild(createNode('text')))
    })
  })
  describe('removeChild()', () => {
    it('should always throw Error', () => {
      assert.throws(() => createNode('text').removeChild(createNode('text')))
    })
  })
  describe('insertBefore()', () => {
    it('should always throw Error', () => {
      assert.throws(() => createNode('text').insertBefore(createNode('text'), createNode('text')))
    })
  })
  describe('text', () => {
    it('should set text property to a string', () => {
      const node = createNode('text')

      node.text = 'test'

      assert.equal(node.text, 'test')
    })
    it('should set text to empty string when assigned null or undefined', () => {
      const node = createNode('text')

      for (const input of [null, '', undefined]) {
        node.text = input
        assert.equal(node.text, '')
      }
    })
    it('should throw error when assign a non-string value', () => {
      const node = createNode('text')

      for (const input of [{}, 3]) {
        assert.throws(() => { node.text = input })
      }
    })
  })
})
