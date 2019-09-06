/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { performance } from 'perf_hooks'
import { ScanCode } from './ScanCode'
import { StandardKey } from './StandardKey'
import { KeyEvent } from '../event/KeyEvent'
import { DeviceEvent } from '../event/DeviceEvent'
import { DeviceAxisEvent } from '../event/DeviceAxisEvent'
import { DeviceButtonEvent } from '../event/DeviceButtonEvent'
import {
  $adapter,
  $mappings,
  $scene,
  $capture,
  $bubble,
  $events,
  $keyToDirectionMap,
  $keyboard
} from '../util/InternalSymbols'
import { KeyMapping } from './KeyMapping'
import { Direction } from '../scene/Direction'

const { now } = performance
const keyboardUuid = 'keyboard'

export const InputManager$attach = (self, stage) => {
  if (self[$adapter]) {
    return
  }

  const adapter = stage[$adapter]

  self[$adapter] = adapter
  self[$keyboard] = adapter.getKeyboard()

  adapter.setCallback('connected', (gamepad) => {
    // updateMapping(gamepad, mappings)

    stage[$events].emit(new DeviceEvent(gamepad, true, now()))
  })

  adapter.setCallback('disconnected', (gamepad) => {
    stage[$events].emit(new DeviceEvent(gamepad, false, now()))
  })

  adapter.setCallback('keyup', (keyboard, button) => {
    const timestamp = now()
    const hardwareEvent = new DeviceButtonEvent(keyboard, button, false, false, timestamp)

    stage[$scene][$bubble](hardwareEvent)

    const mapping = self[$mappings].get(keyboard.uuid)

    if (!mapping || !mapping.hasKey(button)) {
      return
    }

    const keyEvent = new KeyEvent(
      mapping.getKey(button),
      false,
      false,
      mapping.name,
      { device: keyboard, button: button },
      timestamp)

    stage[$scene][$bubble](keyEvent)
  })

  adapter.setCallback('keydown', (keyboard, button, repeat) => {
    const timestamp = now()
    const hardwareEvent = new DeviceButtonEvent(keyboard, button, true, repeat, timestamp)

    stage[$scene][$bubble](hardwareEvent)

    const mapping = self[$mappings].get(keyboard.uuid)

    if (!mapping || !mapping.hasKey(button)) {
      return
    }

    const keyEvent = new KeyEvent(
      mapping.getKey(button),
      true,
      repeat,
      mapping.name,
      { device: keyboard, button: button },
      timestamp)

    stage[$scene][$capture](keyEvent)

    stage[$scene][$bubble](keyEvent)
  })

  adapter.setCallback('buttonup', (gamepad, button) => {
    const timestamp = now()
    const hardwareEvent = new DeviceButtonEvent(gamepad, button, false, false, timestamp)

    stage[$scene][$bubble](hardwareEvent)

    const mapping = self[$mappings].get(gamepad.uuid)

    if (!mapping || !mapping.hasKey(button)) {
      return
    }

    const keyEvent = new KeyEvent(
      self[mapping].getKey(button),
      false,
      false,
      mapping.name,
      { device: gamepad, button: button },
      timestamp)

    stage[$scene][$bubble](keyEvent)
  })

  adapter.setCallback('buttondown', (gamepad, button) => {
    const timestamp = now()
    const hardwareEvent = new DeviceButtonEvent(gamepad, button, true, false, timestamp)

    stage[$scene][$bubble](hardwareEvent)

    const mapping = self[$mappings].get(gamepad.uuid)

    if (!mapping || !mapping.hasKey(button)) {
      return
    }

    const keyEvent = new KeyEvent(
      self[mapping].getKey(button),
      true,
      false,
      mapping.name,
      { device: gamepad, button: button },
      timestamp)

    stage[$scene][$capture](keyEvent)

    stage[$scene][$bubble](keyEvent)
  })

  adapter.setCallback('axismotion', (gamepad, axis, value) => {
    const timestamp = now()
    const hardwareEvent = new DeviceAxisEvent(gamepad, axis, value, timestamp)

    stage[$scene][$bubble](hardwareEvent)

    // TODO: mapping
  })
}

export const InputManager$detach = (self) => {
  if (self[$adapter]) {
    self[$adapter].resetCallbacks()
    self[$adapter] = null
    self[$keyboard] = null
  }
}

/**
 *
 */
export class InputManager {
  constructor () {
    this[$adapter] = null
    this[$keyboard] = null

    const keyboardMapping = new KeyMapping('standard', [
      [StandardKey.UP, ScanCode.UP],
      [StandardKey.RIGHT, ScanCode.RIGHT],
      [StandardKey.DOWN, ScanCode.DOWN],
      [StandardKey.LEFT, ScanCode.LEFT]
    ])

    this[$mappings] = new Map([
      [keyboardUuid, keyboardMapping]
    ])

    this[$keyToDirectionMap] = new Map([
      [StandardKey.UP, Direction.UP],
      [StandardKey.RIGHT, Direction.RIGHT],
      [StandardKey.DOWN, Direction.DOWN],
      [StandardKey.LEFT, Direction.LEFT]
    ])
  }

  /**
   *
   * @returns {Keyboard}
   */
  get keyboard () {
    return this[$keyboard]
  }

  /**
   *
   * @returns {Gamepad[]}
   */
  get gamepads () {
    return this[$adapter] ? this[$adapter].getGamepads() : []
  }

  /**
   *
   * @param uuid
   * @param keyMapping
   */
  addKeyMapping (uuid, keyMapping) {
    if (!uuid || typeof uuid !== 'string') {
      throw Error(`uuid must be a string. Got: '${uuid}'`)
    }

    if (!(keyMapping instanceof KeyMapping)) {
      throw Error('keyMapping must be a KeyMapping instance')
    }

    this[$mappings].set(uuid, keyMapping)
  }

  /**
   *
   * @param uuid
   * @returns {*}
   */
  getKeyMapping (uuid) {
    return this[$mappings].get(uuid) || null
  }

  /**
   *
   * @param uuid
   */
  removeKeyMapping (uuid) {
    this[$mappings].delete(uuid)
  }

  /**
   *
   * @param csv
   */
  addGameControllerMappings (csv) {
    const adapter = this[$adapter]

    if (!adapter.addGameControllerMappings) {
      throw Error('addGameControllerMappings() is unavailable')
    }

    // TODO: update key mappings

    return adapter.addGameControllerMappings(csv)
  }
}

// const updateMapping = (device, mappings) => {
//   const { uuid, mapping } = device
//
//   !mappings.has(uuid) && mapping && mappings.set(uuid, mapping)
// }
