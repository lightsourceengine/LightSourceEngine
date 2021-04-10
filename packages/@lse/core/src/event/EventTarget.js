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
