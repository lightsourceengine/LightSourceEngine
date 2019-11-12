/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { afterSceneTest, beforeSceneTest, createNode } from '..'
import { assert } from 'chai'
import { BoxSceneNode } from '../../src/addon'

describe('BoxSceneNode', () => {
  before(beforeSceneTest)
  after(afterSceneTest)
  describe('constructor()', () => {
    it('should create uninitialized node when passed an invalid Scene', () => {
      for (const input of [null, undefined, {}]) {
        const obj = new BoxSceneNode(input)

        assert.isNull(obj.scene)
        obj.destroy()
      }
    })
  })
  describe('appendChild()', () => {
    it('should append a child node', () => {
      const node = createNode('box')
      const child = createNode('img')

      node.appendChild(child)

      assert.lengthOf(node.children, 1)
      assert.strictEqual(node.children[0], child)
      assert.strictEqual(node.children[0].parent, node)
    })
    it('should throw Error when adding self as child', () => {
      const node = createNode('box')

      assert.throws(() => node.appendChild(node))
    })
    it('should throw Error when adding the same child twice', () => {
      const node = createNode('box')
      const child = createNode('img')

      node.appendChild(child)
      assert.throws(() => node.appendChild(node))
    })
    it('should throw Error when node is invalid', () => {
      const node = createNode('box')

      for (const child of [null, undefined, '', {}]) {
        assert.throws(() => node.appendChild(child))
      }
    })
  })
  describe('removeChild()', () => {
    it('should remove a child node', () => {
      const node = createNode('box')
      const child = createNode('img')

      node.appendChild(child)
      assert.lengthOf(node.children, 1)

      node.removeChild(child)

      assert.lengthOf(node.children, 0)
      assert.isNull(child.parent)
    })
    it('should throw Error when removing self', () => {
      const node = createNode('box')

      assert.throws(() => node.removeChild(node))
    })
    it('should throw Error when removing a node that is not a child', () => {
      const node = createNode('box')
      const anotherNode = createNode('img')

      assert.throws(() => node.removeChild(anotherNode))
    })
    it('should throw Error when node is invalid', () => {
      const node = createNode('box')

      for (const child of [null, undefined, '', {}]) {
        assert.throws(() => node.removeChild(child))
      }
    })
  })
  describe('insertBefore()', () => {
    it('should insert before a child node', () => {
      const node = createNode('box')
      const child1 = createNode('img')
      const child2 = createNode('img')

      node.appendChild(child1)
      node.insertBefore(child2, child1)

      assert.lengthOf(node.children, 2)
      assert.strictEqual(node.children[0], child2)
      assert.strictEqual(node.children[1], child1)
    })
    it('should throw Error when insert node is invalid', () => {
      const node = createNode('box')
      const child1 = createNode('img')

      node.appendChild(child1)

      for (const input of [null, undefined, '', {}]) {
        assert.throws(() => node.insertBefore(input, child1))
      }
    })
    it('should throw Error when inserting self', () => {
      const node = createNode('box')
      const child1 = createNode('img')

      node.appendChild(child1)

      assert.throws(() => node.insertBefore(node, child1))
    })
    it('should throw Error when before is invalid', () => {
      const node = createNode('box')

      node.appendChild(createNode('img'))

      for (const input of [null, undefined, '', {}]) {
        assert.throws(() => node.insertBefore(createNode('img'), input))
      }
    })
    it('should throw Error when before is self', () => {
      const node = createNode('box')

      node.appendChild(createNode('img'))

      assert.throws(() => node.insertBefore(createNode('img'), node))
    })
  })
})
