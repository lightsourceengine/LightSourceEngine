/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { EventTarget } from '../../src/event/EventTarget.js'
import sinon from 'sinon'

const { assert } = chai
const testEventName = 'on-test'
const testEventSymbol = Symbol.for('on-test')

const invalidEventNames = [0, [], {}, null, undefined, NaN, '', 'blah', Symbol('blah')]

describe('EventTarget', () => {
  let target
  beforeEach(() => {
    target = new EventTarget([testEventName])
  })
  afterEach(() => {
    target = null
  })
  describe('dispatchEvent()', () => {
    it('should dispatch event to callback', () => {
      const callback = sinon.fake()

      target.on(testEventName, callback)
      target.dispatchEvent({ $type: testEventSymbol })
      assert.isTrue(callback.called)
    })
    it('should throw error when event is invalid', () => {
      for (const input of invalidEventNames) {
        assert.throws(() => target.dispatchEvent(input))
      }
    })
    it('should throw error when event.$type id is invalid', () => {
      for (const input of invalidEventNames) {
        assert.throws(() => target.dispatchEvent({ $type: input }))
      }
    })
  })
  describe('on()', () => {
    it('should dispatch event to callback', () => {
      const callback = sinon.fake()
      const event = { $type: testEventSymbol }

      target.on(testEventName, callback)
      target.dispatchEvent(event)
      assert.isTrue(callback.calledWith(event))
    })
    it('should throw error when event id is invalid', () => {
      for (const name of invalidEventNames) {
        assert.throws(() => target.on(name, () => {}))
      }
    })
  })
  describe('once()', () => {
    it('should dispatch event to callback only once', () => {
      const callback = sinon.fake()
      const event = { $type: testEventSymbol }

      target.once(testEventName, callback)
      target.dispatchEvent(event)
      assert.isTrue(callback.calledWith(event))
      callback.resetHistory()
      target.dispatchEvent(event)
      assert.isFalse(callback.called)
    })
    it('should throw error when event id is invalid', () => {
      for (const name of invalidEventNames) {
        assert.throws(() => target.on(name, () => {}))
      }
    })
  })
})
