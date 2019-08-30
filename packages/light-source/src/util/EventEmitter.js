/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Custom EventEmitter.
 *
 * - Removes the overhead of the builtin EventEmitter.
 * - Supports Symbol and string event names.
 * - Dispatch works that same as node's builtin EventEmitter.
 * - Removing listeners puts a null entry in the listener array. When emit is called, it may compact the listener
 *   array to remove nulls.
 *
 * @ignore
 */
export class EventEmitter {
  /**
   * @param events List of event names (Symbol or string) supported by this emitter.
   */
  constructor (events) {
    if (!Array.isArray(events)) {
      throw Error('Expected an array of events')
    }

    for (const event of events) {
      if (typeof event === 'symbol') {
        if (!Symbol.keyFor(event)) {
          throw Error('Expected a symbol created from Symbol.for()')
        }
        this[event] = []
      } else if (event === 'string') {
        this[Symbol.for(event)] = []
      } else {
        throw Error(`Expected event name to be a string or symbol. Got ${event}`)
      }
    }
  }

  /**
   * Dispatch an event.
   *
   * @param {Object} event An object with a 'type' property that is a registered event name Symbol
   */
  emit (event) {
    (event && typeof event.type === 'symbol') || throwExpectedEventObject(event)

    const listeners = this[event.type]

    if (listeners) {
      let needsCompact = 0

      for (const listener of listeners) {
        if (listener) {
          listener(event)
        } else {
          needsCompact++
        }
      }

      if (needsCompact) {
        if (needsCompact === listeners.length) {
          listeners.length = 0
        } else if (needsCompact > 8) {
          this[event.type] = listeners.filter(Boolean)
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
    const sym = typeof id === 'string' ? Symbol.for(id) : id

    typeof sym === 'symbol' || throwExpectedSymbol(id)
    typeof listener === 'function' || throwExpectedFunction(id)
    this[sym] || throwUnregisteredEventType(sym)

    this[sym].push(listener)
  }

  /**
   * Add a listener that will be removed after the next emit.
   *
   * @param {string|Symbol} id Event name to listen to
   * @param {function} listener Listener to register. This will be called back with an Event object argument
   * @throws Error when id is invalid (not a string, Symbol or unregistered) or listener is not a function
   */
  once (id, listener) {
    const sym = typeof id === 'string' ? Symbol.for(id) : id

    typeof sym === 'symbol' || throwExpectedSymbol()
    typeof listener === 'function' || throwExpectedFunction()

    const wrapper = event => {
      this.off(sym, wrapper)
      listener(event)
    }

    this.on(sym, wrapper)
  }

  /**
   * Remove a listener.
   *
   * @param {string|Symbol} id Event name to listen to
   * @param {function} listener Listener to unregister
   */
  off (id, listener) {
    const sym = typeof id === 'string' ? Symbol.for(id) : id

    typeof sym === 'symbol' || throwExpectedSymbol()

    const listeners = this[sym]

    if (listeners) {
      const len = listeners.length

      for (let i = 0; i < len; i++) {
        if (listeners[i] === listener) {
          listeners[i] = null
        }
      }
    }
  }
}

const throwExpectedSymbol = arg => { throw Error(`Expected event type to be a Symbol. Got ${arg}`) }
const throwExpectedFunction = arg => { throw Error(`Expected listener to be a Function. Got ${arg}`) }
const throwExpectedEventObject = arg => { throw Error(`Expected event to be an Event object. Got ${arg}`) }
const throwUnregisteredEventType = arg => { throw Error(`Event type ${typeof arg === 'symbol' ? 'symbol' : arg} has not been registered.`) }
