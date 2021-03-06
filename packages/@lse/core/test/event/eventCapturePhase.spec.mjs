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
import { eventCapturePhase } from '../../src/event/eventCapturePhase.mjs'
import { Key } from '../../src/input/Key.mjs'
import { afterSceneTest, beforeSceneTest } from '../test-env.mjs'
import { waypoint } from '../../src/scene/waypoint.mjs'
import { createKeyDownEvent } from '../../src/event/index.mjs'
import { Direction } from '../../src/input/Direction.mjs'

const { assert } = chai

describe('eventCapturePhase()', () => {
  let scene
  before(() => {
    scene = beforeSceneTest()
    setupSceneRoot(scene)
  })
  after(() => { scene = afterSceneTest() })
  it('should stop propagation after move right', () => {
    const event = rightKeyEvent()

    eventCapturePhase(scene.activeNode, Direction.RIGHT, event)
    assert.isTrue(event.hasStopPropagation())
    assert.strictEqual(scene.root.children[0].children[1], scene.activeNode)
  })
  it('should stop propagation after failed move left', () => {
    const event = leftKeyEvent()

    eventCapturePhase(scene.activeNode, Direction.LEFT, event)
    assert.isTrue(event.hasStopPropagation())
    assert.strictEqual(scene.root.children[0].children[0], scene.activeNode)
  })
  it('should be a noop if scene has no active node', () => {
    const event = rightKeyEvent()

    scene.activeNode.blur()
    assert.isNull(scene.activeNode)
    eventCapturePhase(scene.activeNode, Direction.RIGHT, event)
    assert.isFalse(event.hasStopPropagation())
  })
})

const setupSceneRoot = (scene) => {
  const group = scene.createNode('box')
  const child1 = scene.createNode('img')
  const child2 = scene.createNode('img')

  child1.focusable = true
  child2.focusable = true
  group.waypoint = waypoint('horizontal')

  group.appendChild(child1)
  group.appendChild(child2)
  scene.root.appendChild(group)

  child1.focus()
}

const rightKeyEvent = () => createKeyDownEvent(null, Key.DPAD_RIGHT, false)

const leftKeyEvent = () => createKeyDownEvent(null, Key.DPAD_LEFT, false)
