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
import { BoxSceneNode } from '../../src/scene/SceneNode.mjs'

const { assert } = chai

describe('BoxSceneNode', () => {
  let scene
  beforeEach(() => { scene = beforeSceneTest() })
  afterEach(() => { scene = afterSceneTest() })
  describe('constructor()', () => {
    it('should create uninitialized node when passed an invalid Scene', () => {
      for (const input of [null, undefined, {}]) {
        assert.throws(() => new BoxSceneNode(input))
      }
    })
  })
  describe('appendChild()', () => {
    it('should append a child node', () => {
      const node = scene.createNode('box')
      const child = scene.createNode('img')

      node.appendChild(child)

      assert.lengthOf(node.children, 1)
      assert.strictEqual(node.children[0], child)
      assert.strictEqual(node.children[0].parent, node)
    })
    it('should throw Error when adding self as child', () => {
      const node = scene.createNode('box')

      assert.throws(() => node.appendChild(node))
    })
    it('should throw Error when adding the same child twice', () => {
      const node = scene.createNode('box')
      const child = scene.createNode('img')

      node.appendChild(child)
      assert.throws(() => node.appendChild(node))
    })
    it('should throw Error when node is invalid', () => {
      const node = scene.createNode('box')

      for (const child of [null, undefined, '', {}]) {
        assert.throws(() => node.appendChild(child))
      }
    })
  })
  describe('removeChild()', () => {
    it('should remove a child node', () => {
      const node = scene.createNode('box')
      const child = scene.createNode('img')

      node.appendChild(child)
      assert.lengthOf(node.children, 1)

      node.removeChild(child)

      assert.lengthOf(node.children, 0)
      assert.isNull(child.parent)
    })
    it('should be a no-op when child is self', () => {
      const node = scene.createNode('box')

      node.removeChild(node)

      assert.lengthOf(node.children, 0)
    })
    it('should be a no-op attempting to remove a node that is not a child', () => {
      const node = scene.createNode('box')
      const anotherNode = scene.createNode('img')

      node.removeChild(anotherNode)
    })
    it('should be a no-op attempting to remove an invalid node', () => {
      const node = scene.createNode('box')

      for (const child of [null, undefined, '', {}]) {
        node.removeChild(child)
      }
    })
  })
  describe('insertBefore()', () => {
    it('should insert before a child node', () => {
      const node = scene.createNode('box')
      const child1 = scene.createNode('img')
      const child2 = scene.createNode('img')

      node.appendChild(child1)
      node.insertBefore(child2, child1)

      assert.lengthOf(node.children, 2)
      assert.strictEqual(node.children[0], child2)
      assert.strictEqual(node.children[1], child1)
    })
    it('should throw Error when insert node is invalid', () => {
      const node = scene.createNode('box')
      const child1 = scene.createNode('img')

      node.appendChild(child1)

      for (const input of [null, undefined, '', {}]) {
        assert.throws(() => node.insertBefore(input, child1))
      }
    })
    it('should throw Error when inserting self', () => {
      const node = scene.createNode('box')
      const child1 = scene.createNode('img')

      node.appendChild(child1)

      assert.throws(() => node.insertBefore(node, child1))
    })
    it('should throw Error when before is invalid', () => {
      const node = scene.createNode('box')

      node.appendChild(scene.createNode('img'))

      for (const input of [null, undefined, '', {}]) {
        assert.throws(() => node.insertBefore(scene.createNode('img'), input))
      }
    })
    it('should throw Error when before is self', () => {
      const node = scene.createNode('box')

      node.appendChild(scene.createNode('img'))

      assert.throws(() => node.insertBefore(scene.createNode('img'), node))
    })
  })
})
