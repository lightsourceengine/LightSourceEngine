/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { EventEmitter } from '../util/EventEmitter.js'

const emitter = Symbol('emitter')

/**
 * Used internally for @lse/core objects that broadcast events.
 *
 * @memberof module:@lse/core
 * @hideconstructor
 */
class EventTarget {
  constructor (events) {
    this[emitter] = new EventEmitter(events)
  }

  /**
   * Register a listener with the EventTarget.
   *
   * @param {string|Symbol} eventId Event name to listen to
   * @param {function} listener Function to be called when an event occurs. Called with a single event object containing
   * information about the event
   */
  on (eventId, listener) {
    this[emitter].on(eventId, listener)
  }

  /**
   * Register a listener with the EventTarget.
   *
   * The listener is called once and automatically removed from the internal listener list.
   *
   * @param {string|Symbol} eventId Event name to listen to
   * @param {function} listener Function to be called when an event occurs. Called with a single event object containing
   * information about the event
   */
  once (eventId, listener) {
    this[emitter].once(eventId, listener)
  }

  /**
   * Remove a registered listener.
   *
   * @param {string|Symbol} eventId Event name
   * @param {function} listener a registered listener
   */
  off (eventId, listener) {
    this[emitter].off(eventId, listener)
  }

  /**
   * @ignore
   */
  dispatchEvent (event, defer = false) {
    this[emitter].emitEvent(event, defer)
  }
}

export { EventTarget }
