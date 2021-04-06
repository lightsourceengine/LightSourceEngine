/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import React from 'react'
import { stage } from '@lse/core'
import { TextElement } from '../src/TextElement.js'
import { ReactRenderer } from '../src/ReactRenderer.js'
import './test-env.js'

const { assert } = chai
let root
let reactRenderer

describe('ReactRenderer', () => {
  beforeEach(() => {
    root = stage.$scene.root
    reactRenderer = new ReactRenderer(root)
  })
  afterEach(async () => {
    await reactRendererRender(null)
    root = null
    reactRenderer = null
  })
  describe('render()', () => {
    it('should render a single box element', async () => {
      assert.lengthOf(root.children, 0)

      await reactRendererRender(<box />)

      assert.lengthOf(root.children, 1)
    })
    it('should render a single box element', async () => {
      assert.lengthOf(root.children, 0)

      await reactRendererRender(<box />)

      assert.lengthOf(root.children, 1)
    })
    it('should a Component with a single box element', async () => {
      assert.lengthOf(root.children, 0)

      const TestComponent = () => <box />

      await reactRendererRender(<TestComponent />)

      assert.lengthOf(root.children, 1)
    })
    it('should render a single img element', async () => {
      assert.lengthOf(root.children, 0)

      await reactRendererRender(<img />)

      assert.lengthOf(root.children, 1)
    })
    it('should render a single text element', async () => {
      assert.lengthOf(root.children, 0)

      await reactRendererRender(<text>Test</text>)

      assert.lengthOf(root.children, 1)
    })
    it('should clear all root children when null is rendered', async () => {
      assert.lengthOf(root.children, 0)

      await reactRendererRender(<box />)

      assert.lengthOf(root.children, 1)

      await reactRendererRender(null)

      assert.lengthOf(root.children, 0)
    })
  })
  describe('findElement()', () => {
    it('should find an Element given a Component instance', async () => {
      let capture
      const TestComponent = class extends React.Component { render () { capture = this; return <text>it's me</text> }}

      await reactRendererRender(<TestComponent />)

      const element = reactRenderer.findElement(capture)

      assert.instanceOf(element, TextElement)
      assert.strictEqual(element.node, root.children[0])
      assert.strictEqual(element.node.text, 'it\'s me')
    })
  })
  describe('findNode()', () => {
    it('should find a SceneNode given a Component instance', async () => {
      let capture
      const TestComponent = class extends React.Component { render () { capture = this; return <text>it's me</text> }}

      await reactRendererRender(<TestComponent />)

      const sceneNode = reactRenderer.findSceneNode(capture)

      assert.strictEqual(sceneNode, root.children[0])
      assert.strictEqual(sceneNode.text, 'it\'s me')
    })
  })
})

const reactRendererRender = async (element) => {
  return new Promise((resolve) => reactRenderer.render(element, resolve))
}
