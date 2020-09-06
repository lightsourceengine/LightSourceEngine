/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import React from 'react'
import { afterEachTestCase, beforeEachTestCase, container, renderAsync } from './test-env.js'
import { findElement, findSceneNode, render } from '../src/renderer.js'
import { TextElement } from '../src/TextElement.js'

const { assert } = chai

class TestComponent extends React.Component {
  static content = 'this is a test'

  render () {
    return <text>{TestComponent.content}</text>
  }
}

describe('renderer.js', () => {
  beforeEach(() => {
    beforeEachTestCase()
  })
  afterEach(async () => {
    await afterEachTestCase()
  })
  describe('render()', () => {
    it('should render a text node', async () => {
      await renderAsync(<text>test</text>)

      assert.lengthOf(container().children, 1)
      assert.equal(container().children[0].text, 'test')
    })
    it('should remove nodes synchronously', async () => {
      await renderAsync(<text>test</text>)

      assert.lengthOf(container().children, 1)
      assert.equal(container().children[0].text, 'test')

      const p = renderAsync(null)

      // added nodes are removed synchronously
      assert.lengthOf(container().children, 0)

      // callback when all cleanup is finished
      await p
    })
    it('should throw Error for invalid container', () => {
      for (const invalidContainer of [null, undefined, '', {}, []]) {
        assert.throws(() => render(invalidContainer, <div />, () => {}))
      }
    })
    it('should throw Error for invalid callback', () => {
      for (const invalidCallback of [{}, [], 'hello', 1]) {
        assert.throws(() => render(container(), <div />, invalidCallback))
      }
    })
    it('should accept null or undefined callback', () => {
      for (const noCallback of [null, undefined]) {
        render(container(), <div />, noCallback)
      }
    })
    it('should always call callback for null element', async () => {
      await renderAsync(<div />)
      await renderAsync(null)
      await renderAsync(null)
    })
  })
  describe('findElement()', () => {
    it('should find the Element by component class instance', async () => {
      const ref = React.createRef()

      await renderAsync(<TestComponent ref={ref} />)

      assert.instanceOf(ref.current, TestComponent)

      const element = findElement(container(), ref.current)

      assert.instanceOf(element, TextElement)
      assert.equal(element.node.text, TestComponent.content)
    })
    it('should throw Error for invalid container', () => {
      for (const invalidContainer of [null, undefined, '', {}, []]) {
        assert.throws(() => findElement(invalidContainer, null))
      }
    })
  })
  describe('findSceneNode()', () => {
    it('should find the SceneNode by component class instance', async () => {
      const ref = React.createRef()

      await renderAsync(<TestComponent ref={ref} />)

      assert.instanceOf(ref.current, TestComponent)

      const sceneNode = findSceneNode(container(), ref.current)

      assert.isNotNull(sceneNode)
      assert.equal(sceneNode.text, TestComponent.content)
    })

    it('should throw Error for invalid container', () => {
      for (const invalidContainer of [null, undefined, '', {}, []]) {
        assert.throws(() => findSceneNode(invalidContainer, null))
      }
    })
  })
})
