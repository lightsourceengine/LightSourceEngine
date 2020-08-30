/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest } from '../test-env'
import { assert } from 'chai'
import { BoxSceneNode } from '../../src/scene/SceneNode'

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
    xit('should throw Error when adding self as child', () => {
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
    xit('should throw Error when removing self', () => {
      const node = scene.createNode('box')

      assert.throws(() => node.removeChild(node))
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
    xit('should throw Error when inserting self', () => {
      const node = scene.createNode('box')
      const child1 = scene.createNode('img')

      node.appendChild(child1)

      assert.throws(() => node.insertBefore(node, child1))
    })
    xit('should throw Error when before is invalid', () => {
      const node = scene.createNode('box')

      node.appendChild(scene.createNode('img'))

      for (const input of [null, undefined, '', {}]) {
        assert.throws(() => node.insertBefore(scene.createNode('img'), input))
      }
    })
    xit('should throw Error when before is self', () => {
      const node = scene.createNode('box')

      node.appendChild(scene.createNode('img'))

      assert.throws(() => node.insertBefore(scene.createNode('img'), node))
    })
  })
})
