/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { eventCapturePhase } from '../../src/event/eventCapturePhase'
import { performance } from 'perf_hooks'
import { KeyEvent } from '../../src/event/KeyEvent'
import { Direction } from '../../src/input/Direction'
import { Key } from '../../src/input/Key'
import { Mapping } from '../../src/input/Mapping'
import { afterSceneTest, beforeSceneTest } from '../test-env'
import { waypoint } from '../../src/scene/waypoint'

describe('eventCapturePhase()', () => {
  let scene
  before(() => {
    scene = beforeSceneTest()
    setupSceneRoot(scene)
  })
  after(() => { scene = afterSceneTest() })
  it('should stop propagation after move right', () => {
    const event = rightKeyEvent()

    eventCapturePhase(scene.stage, scene, event)
    assert.isTrue(event.cancelled)
    assert.strictEqual(scene.root.children[0].children[1], scene.activeNode)
  })
  it('should stop propagation after failed move left', () => {
    const event = leftKeyEvent()

    eventCapturePhase(scene.stage, scene, event)
    assert.isTrue(event.cancelled)
    assert.strictEqual(scene.root.children[0].children[0], scene.activeNode)
  })
  it('should be a noop if scene has not active node', () => {
    const event = rightKeyEvent()

    scene.activeNode = null
    eventCapturePhase(scene.stage, scene, event)
    assert.isFalse(event.cancelled)
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

  scene.activeNode = child1
}

const rightKeyEvent = () => new KeyEvent(Key.RIGHT, true, false, Mapping.STANDARD, Direction.RIGHT, {}, performance.now())

const leftKeyEvent = () => new KeyEvent(Key.LEFT, true, false, Mapping.STANDARD, Direction.LEFT, {}, performance.now())
