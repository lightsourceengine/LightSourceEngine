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
import sinon from 'sinon'
import { BoxSceneNode, TextSceneNode, ImageSceneNode, RootSceneNode } from '../../src/scene/SceneNode.mjs'
import { afterSceneTest, beforeSceneTest } from '../test-env.mjs'

const { assert } = chai

describe('Scene', () => {
  let scene
  beforeEach(() => { scene = beforeSceneTest() })
  afterEach(() => { scene = afterSceneTest() })
  describe('constructor', () => {
    it('should set root style', () => {
      assert.isTrue(scene.root.style.fontSize.isUndefined())
      assert.equal(scene.root.style.backgroundColor, 0xFF000000)
    })
  })
  describe('createNode()', () => {
    it('should create a new SceneNode by tag name', () => {
      const inputs = [
        ['box', BoxSceneNode],
        ['text', TextSceneNode],
        ['img', ImageSceneNode]
      ]

      for (const [tag, cls] of inputs) {
        const node = scene.createNode(tag)
        assert.instanceOf(node, cls)
      }
    })
    it('should throw Error for unsupported tag name', () => {
      for (const tag of ['', 'body', null, undefined]) {
        assert.throws(() => scene.createNode(tag))
      }
    })
  })
  describe('width', () => {
    it('should get renderable width', () => {
      scene.$attach()
      assert.equal(scene.width, 1280)
    })
  })
  describe('height', () => {
    it('should get renderable height', () => {
      scene.$attach()
      assert.equal(scene.height, 720)
    })
  })
  describe('fullscreen', () => {
    it('should get fullscreen state', () => {
      scene.$attach()
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
      assert.instanceOf(scene.root, RootSceneNode)
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

      node.focusable = true
      node.onFocus = sinon.stub()
      scene.root.appendChild(node)
      node.focus()
      assert.strictEqual(scene.activeNode, node)
      assert.isTrue(node.onFocus.called)
    })
    it('should update active node and call onFocus on new focus', () => {
      const node = scene.createNode('box')

      node.focusable = true
      node.onFocus = sinon.stub()
      scene.root.appendChild(node)
      node.focus()
      assert.strictEqual(scene.activeNode, node)
      assert.isTrue(node.onFocus.called)
    })
    it('should set to null', () => {
      const node = scene.createNode('box')

      node.focusable = true
      node.onBlur = sinon.stub()
      scene.root.appendChild(node)
      node.focus()

      scene.activeNode.blur()
      assert.isNull(scene.activeNode)
      assert.isTrue(node.onBlur.called)
    })
  })
  describe('requestAnimationFrame()', () => {
    it('should return requestId > 0', () => {
      const requestId = scene.requestAnimationFrame(() => {})

      assert.isAbove(requestId, 0)
    })
    it('should call callback on the next frame', (done) => {
      scene.requestAnimationFrame(() => done())
    })
    it('should be schedulable in callback', (done) => {
      scene.requestAnimationFrame(() => {
        scene.requestAnimationFrame(() => done())
      })
    })
  })
  describe('cancelAnimationFrame()', () => {
    it('should cancel a request', () => {
      const requestId = scene.requestAnimationFrame(() => {})

      scene.cancelAnimationFrame(requestId)
    })
    it('should cancel within callback', (done) => {
      scene.requestAnimationFrame(() => {
        scene.cancelAnimationFrame(toCancel)
      })

      const toCancel = scene.requestAnimationFrame(() => {
        assert.fail('should not have been called')
      })

      scene.requestAnimationFrame(() => done())
    })
  })
})
