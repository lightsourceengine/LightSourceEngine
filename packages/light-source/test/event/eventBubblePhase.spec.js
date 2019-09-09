/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import sinon from 'sinon'
import { eventBubblePhase } from '../../src/event/eventBubblePhase'
import { $emit } from '../../src/util/InternalSymbols'
import { FocusEvent } from '../../src/event/FocusEvent'
import { performance } from 'perf_hooks'

describe('eventBubblePhase()', () => {
  let stage
  let scene
  let event
  beforeEach(() => {
    stage = {
      [$emit]: sinon.stub()
    }
    scene = {
      [$emit]: sinon.stub()
    }
    event = new FocusEvent(performance.now())
  })
  afterEach(() => {
    stage = null
    scene = null
    event = null
  })
  it('should bubble through scene and stage', () => {
    eventBubblePhase(stage, scene, event)

    assert.isTrue(scene[$emit].calledOnce)
    assert.isTrue(stage[$emit].calledOnce)
  })
  it('should stop propagation at scene', () => {
    scene[$emit] = stopPropagation

    eventBubblePhase(stage, scene, event)

    assert.isFalse(stage[$emit].called)
    assert.isTrue(event.cancelled)
  })
  it('should stop propagation at stage', () => {
    stage[$emit] = stopPropagation

    eventBubblePhase(stage, scene, event)

    assert.isTrue(scene[$emit].called)
    assert.isTrue(event.cancelled)
  })
  it('should call onFocus on scene\'s active node', () => {
    scene.activeNode = mockSceneNode()

    eventBubblePhase(stage, scene, event)

    assert.isTrue(scene.activeNode.onFocus.calledOnce)
    assert.isTrue(scene[$emit].calledOnce)
    assert.isTrue(stage[$emit].calledOnce)
  })
  it('should stop propagation at active node', () => {
    scene.activeNode = {
      parent: null,
      onFocus: stopPropagation
    }

    eventBubblePhase(stage, scene, event)

    assert.isFalse(scene[$emit].called)
    assert.isFalse(stage[$emit].called)
  })
})

const stopPropagation = (event) => event.stop()

const mockSceneNode = () => ({ parent: null, onFocus: sinon.stub() })
