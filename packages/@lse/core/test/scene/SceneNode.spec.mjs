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
import { afterSceneTest, beforeSceneTest } from '../test-env.mjs'
import { createStyleClass } from '../../src/style/createStyleClass.mjs'
import { StyleInstance } from '../../src/style/StyleInstance.mjs'
import { StyleClass } from '../../src/style/StyleClass.mjs'

const { assert } = chai

describe('SceneNode', () => {
  let scene
  beforeEach(() => { scene = beforeSceneTest() })
  afterEach(() => { scene = afterSceneTest() })
  describe('focus()', () => {
    it('should focus when focusable = true', () => {
      const box = addBoxToRoot({ focusable: true })

      box.focus()

      assert.isTrue(box.hasFocus())
      assert.strictEqual(scene.activeNode, box)
    })
    it('should not focus when focusable=false', () => {
      const box = addBoxToRoot({ focusable: false })

      box.focus()

      assert.isFalse(box.hasFocus())
      assert.isNull(scene.activeNode)
    })
    it('should call onFocus', () => {
      const box = addBoxToRoot({ focusable: true, onFocus: sinon.stub() })

      box.focus()

      assert.isTrue(box.hasFocus())
      assert.isTrue(box.onFocus.called)
    })
    it('should call parent onFocusIn', () => {
      const box = addBoxToRoot({ focusable: true, onFocusIn: sinon.stub() })

      scene.root.onFocusIn = sinon.stub()

      box.focus()

      assert.isTrue(box.hasFocus())
      assert.isTrue(scene.root.onFocusIn.called)
      assert.isFalse(box.onFocusIn.called)
    })
    it('should not call onFocusIn if focus event is stopped', () => {
      const box = addBoxToRoot({ focusable: true, onFocus: (e) => e.stopPropagation() })

      scene.root.onFocusIn = sinon.stub()

      box.focus()

      assert.isTrue(box.hasFocus())
      assert.isFalse(scene.root.onFocusIn.called)
    })
  })
  describe('blur()', () => {
    it('should clear focus', () => {
      const box = addBoxToRoot({ focusable: true })

      box.focus()
      box.blur()

      assert.isFalse(box.hasFocus())
      assert.isNull(scene.activeNode)
    })
    it('should call onBlur', () => {
      const box = addBoxToRoot({ focusable: true, onBlur: sinon.stub() })

      box.focus()
      box.blur()

      assert.isFalse(box.hasFocus())
      assert.isTrue(box.onBlur.called)
    })
    it('should call onFocusOut on ancestors', () => {
      const box = addBoxToRoot({ focusable: true, onFocusOut: sinon.stub() })

      scene.root.onFocusOut = sinon.stub()

      box.focus()
      box.blur()

      assert.isFalse(box.hasFocus())
      assert.isFalse(box.onFocusOut.called)
      assert.isTrue(scene.root.onFocusOut.called)
    })
  })
  describe('style', () => {
    it('should be an instance of Style', () => {
      const node = scene.createNode('box')

      assert.instanceOf(node.style, StyleInstance)
    })
    it('should update maxLines property', () => {
      const node = scene.createNode('box')

      node.style.maxLines = 10

      assert.equal(node.style.maxLines, 10)
    })
    it('should override maxLines property', () => {
      const node = scene.createNode('box')

      node.class = createStyleClass({ maxLines: 20 })

      node.style.maxLines = 10

      assert.equal(node.style.maxLines, 10)
    })
    it('should not be able to set style', () => {
      const node = scene.createNode('box')

      assert.throws(() => { node.style = {} })
    })
  })
  describe('class', () => {
    it('should set new class', () => {
      const node = scene.createNode('box')

      node.class = createStyleClass({ maxLines: 20 })

      assert.instanceOf(node.class, StyleClass)
      assert.equal(node.style.maxLines, 20)
    })
    it('should replace class', () => {
      const node = scene.createNode('box')

      node.class = createStyleClass({ maxLines: 20 })

      assert.instanceOf(node.class, StyleClass)
      assert.equal(node.style.maxLines, 20)

      node.class = createStyleClass({ maxLines: 30 })

      assert.instanceOf(node.class, StyleClass)
      assert.equal(node.style.maxLines, 30)
    })
    it('should set clear class with null', () => {
      const node = scene.createNode('box')

      node.class = createStyleClass({ maxLines: 20 })

      assert.instanceOf(node.class, StyleClass)

      node.class = null

      assert.isNull(node.class)
    })
    it('should set clear class with undefined', () => {
      const node = scene.createNode('box')

      node.class = createStyleClass({ maxLines: 20 })

      assert.instanceOf(node.class, StyleClass)

      node.class = undefined

      assert.isNull(node.class)
    })
    it('should throw Error for non-StyleClass objects', () => {
      const node = scene.createNode('box')

      for (const input of [{}, '', [], NaN]) {
        assert.throws(() => { node.class = input })
      }
    })
  })
  const addBoxToRoot = (props = {}) => {
    const box = scene.createNode('box')

    for (const prop of Object.keys(props)) {
      box[prop] = props[prop]
    }

    scene.root.appendChild(box)

    return box
  }
})
