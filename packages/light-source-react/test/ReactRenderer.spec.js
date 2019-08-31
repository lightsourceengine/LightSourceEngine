/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import React from 'react'
import { Element } from '../src/Element'
import { renderAsync, root, container, beforeSceneTest, afterSceneTest } from './test-env'

const assertClassName = (obj, expectedClassName) => {
  assert.equal(Object.getPrototypeOf(obj).constructor.name, expectedClassName)
}

const TestComponent = () => <div />

class TestComponentClass extends React.Component {
  static instance

  render () {
    TestComponentClass.instance = this

    return <div />
  }
}

describe('ReactContainer', () => {
  beforeEach(beforeSceneTest)
  afterEach(async () => {
    await afterSceneTest()
    TestComponentClass.instance = null
  })
  describe('render()', () => {
    it('should render a single div element', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<div />)

      assert.lengthOf(root().children, 1)
      assertClassName(root().children[0], 'BoxSceneNode')
    })
    it('should render a single box element', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<box />)

      assert.lengthOf(root().children, 1)
      assertClassName(root().children[0], 'BoxSceneNode')
    })
    it('should a Component with a single div element', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<TestComponent />)

      assert.lengthOf(root().children, 1)
      assertClassName(root().children[0], 'BoxSceneNode')
    })
    it('should render a single img element', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<img />)

      assert.lengthOf(root().children, 1)
      assertClassName(root().children[0], 'ImageSceneNode')
    })
    it('should render a single text element', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<text>Test</text>)

      assert.lengthOf(root().children, 1)
      assertClassName(root().children[0], 'TextSceneNode')
    })
    it('should clear all root children when null is rendered', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<div />)

      assert.lengthOf(root().children, 1)

      await renderAsync(null)

      assert.lengthOf(root().children, 0)
    })
  })
  describe('findElement()', () => {
    it('should find an Element given a Component instance', async () => {
      await renderAsync(<TestComponentClass />)

      const element = container().findElement(TestComponentClass.instance)

      assert.instanceOf(element, Element)
      assert.strictEqual(element.node, root().children[0])
    })
  })
  describe('findNode()', () => {
    it('should find a SceneNode given a Component instance', async () => {
      await renderAsync(<TestComponentClass />)

      assert.strictEqual(container().findSceneNode(TestComponentClass.instance), root().children[0])
    })
  })
  describe('disconnect()', () => {
    it('should remove all associated nodes asynchronously', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<div />)

      assert.lengthOf(root().children, 1)

      await new Promise(resolve => container().disconnect(() => resolve()))

      assert.lengthOf(root().children, 0)
    })
    it('should remove all associated nodes synchronously', async () => {
      assert.lengthOf(root().children, 0)

      await renderAsync(<div />)

      assert.lengthOf(root().children, 1)

      container().disconnect()

      assert.lengthOf(root().children, 0)
    })
  })
})
