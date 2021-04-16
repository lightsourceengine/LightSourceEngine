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
import React from 'react'
import { afterEachTestCase, beforeEachTestCase, container, renderAsync } from './test-env.mjs'
import { findElement, findSceneNode, render } from '../src/renderer.mjs'
import { TextElement } from '../src/TextElement.mjs'

const { assert } = chai

class TestComponent extends React.Component {
  static content = 'this is a test'

  render () {
    return <text>{TestComponent.content}</text>
  }
}

describe('renderer.mjs', () => {
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
        assert.throws(() => render(invalidContainer, <box />, () => {}))
      }
    })
    it('should throw Error for invalid callback', () => {
      for (const invalidCallback of [{}, [], 'hello', 1]) {
        assert.throws(() => render(container(), <box />, invalidCallback))
      }
    })
    it('should accept null or undefined callback', () => {
      for (const noCallback of [null, undefined]) {
        render(container(), <box />, noCallback)
      }
    })
    it('should always call callback for null element', async () => {
      await renderAsync(<box />)
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
