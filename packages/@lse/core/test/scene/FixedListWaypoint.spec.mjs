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
import { FixedListWaypoint } from '../../src/scene/FixedListWaypoint.mjs'
import { afterSceneTest, beforeSceneTest } from '../test-env.mjs'
import { Key } from '../../src/input/Key.mjs'
import { createKeyDownEvent } from '../../src/event/index.mjs'

const { assert } = chai

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
      sendKey(scene, Key.DPAD_RIGHT)
      assert.isOk(scene.root.children[0].children[1])
    })
    it('should stay on left most node on left event', () => {
      setupSceneRoot(scene, new FixedListWaypoint('horizontal'))

      assert.isOk(scene.root.children[0].children[0])
      sendKey(scene, Key.DPAD_LEFT)
      assert.isOk(scene.root.children[0].children[0])
    })
    it('should move to the next element on down event', () => {
      setupSceneRoot(scene, new FixedListWaypoint('vertical'))

      assert.isOk(scene.root.children[0].children[0])
      sendKey(scene, Key.DPAD_DOWN)
      assert.isOk(scene.root.children[0].children[1])
    })
    it('should stay on top most node on up event', () => {
      setupSceneRoot(scene, new FixedListWaypoint('vertical'))

      assert.isOk(scene.root.children[0].children[0])
      sendKey(scene, Key.DPAD_UP)
      assert.isOk(scene.root.children[0].children[0])
    })
  })
})

const sendKey = (scene, key) => scene.activeNode?.$bubble(createKeyDownEvent(scene.activeNode, key, false))

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

  child1.focus()
}
