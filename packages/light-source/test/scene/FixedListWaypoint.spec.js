/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { FixedListWaypoint } from '../../src/scene/FixedListWaypoint'
import { afterSceneTest, beforeSceneTest } from '../test-env'
import { Key } from '../../src/input/Key'
import { Mapping } from '../../src/input/Mapping'

describe('FixedListWaypoint', () => {
  let scene
  beforeEach(() => {
    scene = beforeSceneTest()
    setupSceneRoot(scene)
  })
  afterEach(() => {
    scene = afterSceneTest()
  })
  describe('constructor', () => {
    it('should create a horizontal waypoint', () => {
      const waypoint = new FixedListWaypoint('horizontal')

      assert.equal(waypoint.tag, 'horizontal')
    })
    it('should create a vertical waypoint', () => {
      const waypoint = new FixedListWaypoint('vertical')

      assert.equal(waypoint.tag, 'vertical')
    })
    it('should throw Error for invalid tag', () => {
      assert.throws(() => new FixedListWaypoint('unknown'))
    })
  })
  describe('navigation', () => {
    it('should move to the next element on right event', () => {
      setupSceneRoot(scene, new FixedListWaypoint('horizontal'))

      assert.isOk(scene.root.children[0].children[0])
      sendKey(scene, Key.RIGHT)
      assert.isOk(scene.root.children[0].children[1])
    })
    it('should stay on left most node on left event', () => {
      setupSceneRoot(scene, new FixedListWaypoint('horizontal'))

      assert.isOk(scene.root.children[0].children[0])
      sendKey(scene, Key.LEFT)
      assert.isOk(scene.root.children[0].children[0])
    })
    it('should move to the next element on down event', () => {
      setupSceneRoot(scene, new FixedListWaypoint('vertical'))

      assert.isOk(scene.root.children[0].children[0])
      sendKey(scene, Key.DOWN)
      assert.isOk(scene.root.children[0].children[1])
    })
    it('should stay on top most node on up event', () => {
      setupSceneRoot(scene, new FixedListWaypoint('vertical'))

      assert.isOk(scene.root.children[0].children[0])
      sendKey(scene, Key.UP)
      assert.isOk(scene.root.children[0].children[0])
    })
  })
})

const sendKey = (scene, key) => scene.stage.input.$sendKeyEvent(key, true, false, Mapping.STANDARD)

const setupSceneRoot = (scene, waypoint) => {
  const group = scene.createNode('box')
  const child1 = scene.createNode('img')
  const child2 = scene.createNode('img')

  child1.focusable = true
  child2.focusable = true
  group.waypoint = waypoint

  group.appendChild(child1)
  group.appendChild(child2)
  scene.root.appendChild(group)

  scene.activeNode = child1
}
