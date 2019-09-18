/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import sinon from 'sinon'
import { BoxSceneNode, TextSceneNode, ImageSceneNode } from '../../src/addon'
import { afterSceneTest, beforeSceneTest, createNode } from '..'
import { $attach } from '../../src/util/InternalSymbols'

describe('Scene', () => {
  let scene
  beforeEach(() => {
    scene = beforeSceneTest()
  })
  afterEach(() => {
    scene = afterSceneTest()
  })
  describe('constructor', () => {
    it('should set root style', () => {
      assert.equal(scene.root.style.fontSize.value, 16)
      assert.equal(scene.root.style.backgroundColor, 0)
    })
  })
  describe('createNode()', () => {
    it('should create a new SceneNode by tag name', () => {
      const inputs = [
        ['div', BoxSceneNode],
        ['box', BoxSceneNode],
        ['text', TextSceneNode],
        ['img', ImageSceneNode]
      ]

      for (const input of inputs) {
        assert.instanceOf(createNode(input[0]), input[1])
      }
    })
    it('should throw Error for unsupported tag name', () => {
      for (const tag of ['', 'body', null, undefined]) {
        assert.throws(() => createNode(tag))
      }
    })
  })
  describe('width', () => {
    it('should get renderable width', () => {
      scene[$attach]()
      assert.equal(scene.width, 1280)
    })
  })
  describe('height', () => {
    it('should get renderable height', () => {
      scene[$attach]()
      assert.equal(scene.height, 720)
    })
  })
  describe('fullscreen', () => {
    it('should get fullscreen state', () => {
      scene[$attach]()
      assert.isTrue(scene.fullscreen)
    })
  })
  describe('displayIndex', () => {
    it('should get displayIndex', () => {
      assert.equal(scene.displayIndex, 0)
    })
  })
  describe('root', () => {
    it('should get root', () => {
      assert.strictEqual(Object.getPrototypeOf(scene.root).constructor.name, 'RootSceneNode')
    })
  })
  describe('title', () => {
    it('should return root', () => {
      scene.title = 'App Title'
      assert.strictEqual(scene.title, 'App Title')
    })
  })
  describe('activeNode', () => {
    it('should set active node and call onFocus on new focus', () => {
      const node = scene.createNode('box')

      node.onFocus = sinon.stub()
      scene.root.appendChild(node)
      scene.activeNode = node
      assert.strictEqual(scene.activeNode, node)
      assert.isTrue(node.onFocus.called)
    })
    it('should update active node and call onFocus on new focus', () => {
      const node = scene.createNode('box')

      node.onFocus = sinon.stub()
      scene.root.appendChild(node)
      node.focus()
      assert.strictEqual(scene.activeNode, node)
      assert.isTrue(node.onFocus.called)
    })
    it('should set to null', () => {
      const node = scene.createNode('box')

      node.onBlur = sinon.stub()
      scene.root.appendChild(node)
      node.focus()

      scene.activeNode = null
      assert.isNull(scene.activeNode)
      assert.isTrue(node.onBlur.called)
    })
  })
})
