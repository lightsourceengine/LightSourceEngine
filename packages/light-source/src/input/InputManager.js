/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { EventEmitter } from 'events'
import { performance } from 'perf_hooks'
import { KeyboardEvent } from '../event/KeyboardEvent'
import { GamepadButtonEvent } from '../event/GamepadButtonEvent'
import { GamepadAxisEvent } from '../event/GamepadAxisEvent'
import { GamepadEvent } from '../event/GamepadEvent'

const { now } = performance

const keyboardKeyUpEventType = 'keyboard:keyup'
const keyboardKeyUpEvent = new KeyboardEvent(keyboardKeyUpEventType, true, true)
const keyboardKeyDownEventType = 'keyboard:keydown'
const keyboardKeyDownEvent = new KeyboardEvent(keyboardKeyDownEventType, true, true)
const gamepadButtonUpEventType = 'gamepad:buttonup'
const gamepadButtonUpEvent = new GamepadButtonEvent(gamepadButtonUpEventType)
const gamepadButtonDownEventType = 'gamepad:buttondown'
const gamepadButtonDownEvent = new GamepadButtonEvent(gamepadButtonDownEventType)
const gamepadAxisMotionEventType = 'gamepad:axismotion'
const gamepadAxisMotionEvent = new GamepadAxisEvent(gamepadAxisMotionEventType)
const gamepadConnectedEventType = 'gamepad:connected'
const gamepadConnectedEvent = new GamepadEvent(gamepadConnectedEventType)
const gamepadDisconnectedEventType = 'gamepad:disconnected'
const gamepadDisconnectedEvent = new GamepadEvent(gamepadDisconnectedEventType)

const $stage = Symbol.for('stage')
const $adapter = Symbol.for('adapter')
const $syncStageAdapter = Symbol.for('syncStageAdapter')

export class InputManager {
  constructor () {
    this[$syncStageAdapter](null)
  }

  [$syncStageAdapter] (stage) {
    if (stage) {
      this[$adapter] = stage[$adapter]
      inputEventDispatcher(this[$adapter], new Map(), new EventEmitter())
    } else {
      this[$adapter] = new UnintializedStageAdapter()
    }
  }

  get keyboard () {
    return this[$stage][$adapter].getKeyboard()
  }

  get gamepads () {
    return this[$stage][$adapter].getGamepads()
  }

  addGameControllerMappings (csv) {
    const adapter = this[$adapter]

    if (!adapter.addGameControllerMappings) {
      throw Error('addGameControllerMappings() is unavailable')
    }

    // TODO: update key mappings

    return adapter.addGameControllerMappings(csv)
  }
}

const inputEventDispatcher = (adapter, mappings, emitter) => {
  adapter.setCallback('connected', (gamepad) => {
    updateMapping(gamepad, mappings)
    emitter.emit(
      gamepadConnectedEventType,
      gamepadConnectedEvent._reset(gamepad, now()))
  })

  adapter.setCallback('disconnected', (gamepad) => {
    emitter.emit(
      gamepadDisconnectedEventType,
      gamepadDisconnectedEvent._reset(gamepad, now()))
  })

  adapter.setCallback('keyup', (keyboard, button) => {
    const timestamp = now()

    emitter.emit(
      keyboardKeyUpEventType,
      keyboardKeyUpEvent._reset(keyboard, timestamp, button))
  })

  adapter.setCallback('keydown', (keyboard, button, repeat) => {
    const timestamp = now()

    emitter.emit(
      keyboardKeyDownEventType,
      keyboardKeyDownEvent._reset(keyboard, timestamp, button, true, repeat))
  })

  adapter.setCallback('buttonup', (gamepad, button) => {
    const timestamp = now()

    emitter.emit(
      gamepadButtonUpEventType,
      gamepadButtonUpEvent._reset(gamepad, timestamp, button))
  })

  adapter.setCallback('buttondown', (gamepad, button) => {
    const timestamp = now()

    emitter.emit(
      gamepadButtonDownEventType,
      gamepadButtonDownEvent._reset(gamepad, timestamp, button, true))
  })

  adapter.setCallback('axismotion', (gamepad, axis, value) => {
    const timestamp = now()

    emitter.emit(gamepadAxisMotionEventType, gamepadAxisMotionEvent._reset(gamepad, timestamp, axis, value))
  })
}

const throwStageAdapterNotInitialized = () => {
  throw Error('stage.init() must be called before accessing input')
}

class UnintializedStageAdapter {
  addGameControllerMappings (csv) {
    throwStageAdapterNotInitialized()
  }

  get keyboard () {
    throwStageAdapterNotInitialized()
  }

  get gamepads () {
    throwStageAdapterNotInitialized()
  }
}

const updateMapping = (device, mappings) => {
  const { uuid, mapping } = device

  !mappings.has(uuid) && mapping && mappings.set(uuid, mapping)
}
