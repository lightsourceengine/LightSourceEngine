/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import chai from 'chai'
import { afterSceneTest, beforeSceneTest } from '../test-env.mjs'
import { TextSceneNode } from '../../src/scene/SceneNode.mjs'

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
