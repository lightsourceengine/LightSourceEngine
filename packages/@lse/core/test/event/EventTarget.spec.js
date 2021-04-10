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
import { EventTarget } from '../../src/event/EventTarget.js'
import sinon from 'sinon'

const { assert } = chai
const testEventName = 'test'
const testEventSymbol = Symbol.for('test')

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
