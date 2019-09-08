/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { stage } from 'light-source'
import { renderer, _testOnlyContainers, _testOnlyReconcilers } from '../src/renderer'
import { ReactRenderer } from '../src/ReactRenderer'

let scene

describe('renderer()', () => {
  beforeEach(() => {
    stage.init({ adapter: 'light-source-ref', audioAdapter: 'light-source-ref' })
    scene = stage.createScene()
  })
  afterEach(async () => {
    await new Promise(resolve => _testOnlyReconcilers.has(scene) ? renderer(scene).disconnect(resolve) : resolve())
    stage[Symbol.for('destroy')]()
    scene = null
  })
  it('should create a container given a Scene', () => {
    const container = renderer(scene.root)

    assert.instanceOf(container, ReactRenderer)
    assert.strictEqual(container[Symbol.for('node')], scene.root)
    assert.equal(_testOnlyReconcilers.size, 1)
    assert.equal(_testOnlyContainers.size, 1)
  })
  it('should create a ReactContainer given a SceneNode', () => {
    const container = renderer(scene.root)

    assert.instanceOf(container, ReactRenderer)
    assert.strictEqual(container[Symbol.for('node')], scene.root)
    assert.equal(_testOnlyReconcilers.size, 1)
    assert.equal(_testOnlyContainers.size, 1)
  })
  it('should create 1 Reconciler per Scene', () => {
    const sceneNode1 = scene.createNode('box')
    const sceneNode2 = scene.createNode('box')

    scene.root.appendChild(sceneNode1)
    scene.root.appendChild(sceneNode2)

    assert.instanceOf(renderer(sceneNode1), ReactRenderer)
    assert.instanceOf(renderer(sceneNode2), ReactRenderer)

    assert.equal(_testOnlyReconcilers.size, 1)
    assert.equal(_testOnlyContainers.size, 2)

    renderer(sceneNode1).disconnect()

    assert.equal(_testOnlyReconcilers.size, 1)
    assert.equal(_testOnlyContainers.size, 1)

    renderer(sceneNode2).disconnect()

    assert.equal(_testOnlyReconcilers.size, 0)
    assert.equal(_testOnlyContainers.size, 0)
  })
  it('should throw Error for invalid argument', () => {
    for (const input of [null, undefined, {}, { root: null }, { root: {} }]) {
      assert.throws(() => renderer(input))
    }
  })
  it('should remove Reconciler on Scene destroy', () => {
    renderer(scene)

    assert.equal(_testOnlyReconcilers.size, 1)
    assert.equal(_testOnlyContainers.size, 1)

    stage[Symbol.for('destroy')]()

    assert.equal(_testOnlyReconcilers.size, 0)
    assert.equal(_testOnlyContainers.size, 0)
  })
})
