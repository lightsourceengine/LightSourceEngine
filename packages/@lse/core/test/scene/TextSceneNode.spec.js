/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { afterSceneTest, beforeSceneTest } from '../test-env.js'
import { TextSceneNode } from '../../src/scene/SceneNode.js'

const { assert } = chai

describe('TextSceneNode', () => {
  let scene
  beforeEach(() => { scene = beforeSceneTest() })
  afterEach(() => { scene = afterSceneTest() })
  describe('constructor()', () => {
    it('should create uninitialized node when passed an invalid Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new TextSceneNode(input))
      }
    })
  })
  describe('appendChild()', () => {
    it('should always throw Error', () => {
      assert.throws(() => scene.createNode('text').appendChild(scene.createNode('text')))
    })
  })
  describe('removeChild()', () => {
    it('should be a no-op', () => {
      const node = scene.createNode('text')

      node.removeChild(scene.createNode('box'))
      assert.lengthOf(node.children, 0)
    })
  })
  describe('insertBefore()', () => {
    it('should always throw Error', () => {
      assert.throws(() => scene.createNode('text').insertBefore(scene.createNode('text'), scene.createNode('text')))
    })
  })
  describe('text', () => {
    it('should set text property to a string', () => {
      const node = scene.createNode('text')

      node.text = 'test'

      assert.equal(node.text, 'test')
    })
    it('should set text to empty string when assigned null or undefined', () => {
      const node = scene.createNode('text')

      for (const input of [null, '', undefined]) {
        node.text = input
        assert.equal(node.text, '')
      }
    })
    it('should throw error when assign a non-string value', () => {
      const node = scene.createNode('text')

      for (const input of [{}, 3]) {
        node.text = 'test'
        node.text = input
        assert.isEmpty(node.text)
      }
    })
  })
})
