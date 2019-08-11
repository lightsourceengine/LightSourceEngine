/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { EventEmitter } from '../../src/util/EventEmitter'
import sinon from 'sinon'
import { assert } from 'chai'

const test = Symbol.for('test')
const event = { type: test }

describe('EventEmitter', () => {
  let emitter
  beforeEach(() => {
    emitter = new EventEmitter([test])
  })
  afterEach(() => {
    emitter = null
  })
  describe('constructor()', () => {
    it('should throw Error if passed a non-array', () => {
      for (const input of [null, undefined, {}, 'string', 3]) {
        assert.throws(() => new EventEmitter(input))
      }
    })
    it('should throw Error if passed invalid event names', () => {
      for (const input of [[null], [Symbol('description')], [3], [undefined], [[]], [{}]]) {
        assert.throws(() => new EventEmitter(input))
      }
    })
  })
  describe('on()', () => {
    it('should register listener by Symbol', () => {
      const callback = sinon.fake()

      emitter.on(test, callback)
      emitter.emit(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
    it('should register listener by string', () => {
      const callback = sinon.fake()

      emitter.on('test', callback)
      emitter.emit(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
    it('should register same listener twice', () => {
      const callback = sinon.fake()

      emitter.on(test, callback)
      emitter.on(test, callback)
      emitter.emit(event)

      assert.equal(callback.callCount, 2)
      assert.isTrue(callback.calledWith(event))
    })
    it('should throw Error on bad event type', () => {
      for (const input of [null, undefined, {}, 3, 'str']) {
        assert.throws(() => emitter.on(input, () => {}))
      }
    })
    it('should throw Error on bad event type', () => {
      assert.throws(() => emitter.on(Symbol.for('unregistered'), () => {}))
    })
  })
  describe('off()', () => {
    it('should remove listener by Symbol', () => {
      const callback = sinon.fake()

      emitter.on(test, callback)
      emitter.off(test, callback)
      emitter.emit(event)

      assert.isFalse(callback.called)
    })
    it('should remove listener by string', () => {
      const callback = sinon.fake()

      emitter.on('test', callback)
      emitter.off('test', callback)
      emitter.emit(event)

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
      emitter.emit(event)
      emitter.emit(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
    it('should call listener once by string and remove it', () => {
      const callback = sinon.fake()

      emitter.once('test', callback)
      emitter.emit(event)
      emitter.emit(event)

      assert.equal(callback.callCount, 1)
      assert.isTrue(callback.calledWith(event))
    })
  })
  describe('emit()', () => {
    it('should throw Error when passed an invalid event', () => {
      for (const input of [undefined, null, {}, 3, 'str']) {
        assert.throws(() => emitter.emit(input))
      }
    })
  })
})
