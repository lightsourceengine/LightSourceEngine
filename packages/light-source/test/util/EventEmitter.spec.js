/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { EventEmitter } from '../../src/util'
import sinon from 'sinon'
import { assert } from 'chai'

const test = Symbol.for('test')
const event = { $type: test }

describe('EventEmitter', () => {
  let emitter
  beforeEach(() => {
    emitter = new EventEmitter([test])
  })
  afterEach(() => {
    emitter = null
  })
  describe('on()', () => {
    it('should register listener by Symbol', () => {
      const callback = sinon.fake()

      emitter.on(test, callback)
      emitter.emitEvent(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
    it('should register listener by string', () => {
      const callback = sinon.fake()

      emitter.on('test', callback)
      emitter.emitEvent(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
    it('should register same listener twice', () => {
      const callback = sinon.fake()

      emitter.on(test, callback)
      emitter.on(test, callback)
      emitter.emitEvent(event)

      assert.equal(callback.callCount, 2)
      assert.isTrue(callback.calledWith(event))
    })
    it('should throw Error on bad event type', () => {
      for (const input of [null, undefined, {}, 3]) {
        assert.throws(() => emitter.on(input, () => {}))
      }
    })
  })
  describe('off()', () => {
    it('should remove listener by Symbol', () => {
      const callback = sinon.fake()

      emitter.on(test, callback)
      emitter.off(test, callback)
      emitter.emitEvent(event)

      assert.isFalse(callback.called)
    })
    it('should remove listener by string', () => {
      const callback = sinon.fake()

      emitter.on('test', callback)
      emitter.off('test', callback)
      emitter.emitEvent(event)

      assert.isFalse(callback.called)
    })
    it('should be a no-op to call with a non-function object', () => {
      for (const input of [null, undefined, {}, 3, 'str']) {
        emitter.off(test, input)
      }
    })
    it('should be a no-op to call with an unregistered function', () => {
      emitter.off(test, () => {})
    })
  })
  describe('once()', () => {
    it('should call listener once by Symbol and remove it', () => {
      const callback = sinon.fake()

      emitter.once(test, callback)
      emitter.emitEvent(event)
      emitter.emitEvent(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
    it('should call listener once by string and remove it', () => {
      const callback = sinon.fake()

      emitter.once('test', callback)
      emitter.emitEvent(event)
      emitter.emitEvent(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
  })
  describe('hasListeners()', () => {
    it('should return false when not listeners', () => {
      assert.isFalse(emitter.hasListeners(test))
    })
    it('should return true when a listener is registered', () => {
      emitter.once(test, () => {})
      assert.isTrue(emitter.hasListeners(test))
    })
    it('should return false after all listeners unregistered', () => {
      emitter.once(test, () => {})
      assert.isTrue(emitter.hasListeners(test))
      emitter.emit(test)
      assert.isFalse(emitter.hasListeners(test))
    })
  })
  describe('emitEvent()', () => {
    it('should throw Error when passed an invalid event', () => {
      for (const input of [undefined, null, {}, 3, 'str']) {
        assert.throws(() => emitter.emitEvent(input))
      }
    })
    it('should continue if listener throws exception', () => {
      let callback = sinon.fake()

      emitter.once(test, () => { throw Error() })
      emitter.once(test, callback)
      emitter.emitEvent(event)

      assert.isTrue(callback.called)
    })
    it('should call listeners late when defer = true', (done) => {
      let callback = sinon.fake()

      emitter.once(test, callback)
      emitter.once(test, () => { assert.isTrue(callback.called); done() })
      emitter.emitEvent(event, true)

      assert.isFalse(callback.called)
    })
    it('should call listeners when defer = false', () => {
      let callback = sinon.fake()

      emitter.once(test, callback)
      emitter.emitEvent(event, false)

      assert.isTrue(callback.called)
    })
  })
})
