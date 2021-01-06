/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { eventCapturePhase } from '../../src/event/eventCapturePhase.js'
import { Key } from '../../src/input/Key.js'
import { afterSceneTest, beforeSceneTest } from '../test-env.js'
import { waypoint } from '../../src/scene/waypoint.js'
import { KeyDownEvent } from '../../src/event/index.js'
import { Direction } from '../../src/input/Direction.js'

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

const rightKeyEvent = () => KeyDownEvent(null, Key.DPAD_RIGHT, false)

const leftKeyEvent = () => KeyDownEvent(null, Key.DPAD_LEFT, false)
