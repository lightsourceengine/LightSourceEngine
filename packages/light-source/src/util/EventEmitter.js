/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { logger } from '../addon'

/**
 * Custom EventEmitter.
 *
 * - Removes the overhead of the builtin EventEmitter.
 * - Supports Symbol and string event names.
 * - Dispatch works the same as node's builtin EventEmitter.
 * - Removing listeners puts a null entry in the listener array. When emit is called, it may compact the listener
 *   array to remove nulls.
 *
 * @ignore
 */
export class EventEmitter {
  constructor (eventTypes = []) {
    for (let eventType of eventTypes) {
      if (typeof eventType === 'symbol') {
        if (!Symbol.keyFor(eventType)) {
          throw TypeError('event symbol must be created using Symbol.for()')
        }
      } else if (typeof eventType === 'string') {
        eventType = Symbol.for(eventType)
      } else {
        throw TypeError('event must be s string or symbol')
      }

      this[eventType] = []
    }
  }

  /**
   * Dispatch an event.
   *
   * @param {Event} event The event to dispatch.
   * @param {boolean} defer if true, queue the emit on the microtask queue; otherwise, emit immediately
   */
  emitEvent (event, defer = false) {
    defer ? queueMicrotask(() => this.emit(event.$type, event)) : this.emit(event.$type, event)
  }

  /**
   * Dispatch an event.
   *
   * @param id {string|symbol} The event type.
   * @param args {...*} Arguments to be forwarded to listeners.
   */
  emit (id, ...args) {
    if (typeof id === 'string') {
      id = Symbol.for(id)
    }

    typeof id === 'symbol' || throwExpectedSymbolOrString(id)

    const listeners = this[id]
    const { length } = listeners

    if (length) {
      let needsCompact = 0

      for (let i = 0; i < length; i++) {
        try {
          listeners[i]?.(...args)
        } catch (e) {
          logger.error(`unhandled listener error: ${e.message}`, 'emit()')
        }

        if (!listeners[i]) {
          needsCompact++
        }
      }

      if (needsCompact) {
        if (needsCompact === listeners.length) {
          listeners.length = 0
        } else if (needsCompact > 8) {
          this[id] = listeners.filter(Boolean)
        }
      }
    }
  }

  /**
   * Add a listener.
   *
   * @param {string|Symbol} id Event name to listen to
   * @param {function} listener Listener to register. This will be called back with an Event object argument
   * @throws Error when id is invalid (not a string, Symbol or unregistered) or listener is not a function
   */
  on (id, listener) {
    if (typeof id === 'string') {
      id = Symbol.for(id)
    } else if (typeof id !== 'symbol') {
      throwExpectedSymbolOrString()
    }

    typeof listener === 'function' || throwExpectedFunction(id)

    this[id].push(listener)
  }

  /**
   * Add a listener that will be called once.
   *
   * @param {string|Symbol} id Event name to listen to
   * @param {function} listener Listener to register. This will be called back with an Event object argument
   * @throws Error when id is invalid (not a string, Symbol or unregistered) or listener is not a function
   */
  once (id, listener) {
    if (typeof id === 'string') {
      id = Symbol.for(id)
    } else if (typeof id !== 'symbol') {
      throwExpectedSymbolOrString()
    }

    typeof listener === 'function' || throwExpectedFunction()

    const wrapper = event => {
      this.off(id, wrapper)
      listener(event)
    }

    this.on(id, wrapper)
  }

  /**
   * Remove a listener.
   *
   * If the listener arg is not a function or a function not registered with this emitter, the call is a no-op.
   *
   * @param {string|Symbol} id Event name to listen to
   * @param {function} listener Listener to unregister
   */
  off (id, listener) {
    if (typeof id === 'string') {
      id = Symbol.for(id)
    } else if (typeof id !== 'symbol') {
      return
    }

    const listeners = this[id]
    const { length } = listeners

    for (let i = 0; i < length; i++) {
      if (listeners[i] === listener) {
        listeners[i] = null
      }
    }
  }

  /**
   * Checks if any listeners are registered for the given event type.
   *
   * @param id {string|symbol} The event type.
   * @returns {boolean} true if more than 0 registered listeners; otherwise, false
   */
  hasListeners (id) {
    if (typeof id === 'string') {
      id = Symbol.for(id)
    } else if (typeof id !== 'symbol') {
      return false
    }

    return this[id].length > 0
  }
}

const throwExpectedSymbolOrString = arg => { throw Error(`Expected event type to be a symbol or string. Got ${arg}`) }
const throwExpectedFunction = arg => { throw Error(`Expected listener to be a Function. Got ${arg}`) }
