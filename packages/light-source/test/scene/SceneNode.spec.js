/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import sinon from 'sinon'
import { afterSceneTest, beforeSceneTest } from '../test-env'

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
  const addBoxToRoot = (props = {}) => {
    const box = scene.createNode('box')

    for (const prop of Object.keys(props)) {
      box[prop] = props[prop]
    }

    scene.root.appendChild(box)

    return box
  }
})
