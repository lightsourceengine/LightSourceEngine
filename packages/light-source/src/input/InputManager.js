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
import { $adapter, $bind, $unbind, $mappings, $scene, $capture, $bubble, $events } from '../util/InternalSymbols'

const { now } = performance
const keyboardUuid = 'keyboard'

export class InputManager {
  constructor () {
    this[$adapter] = unintializedStageAdapter

    const keyboardMapping = new Map([
      [ScanCode.UP, StandardKey.UP],
      [ScanCode.RIGHT, StandardKey.RIGHT],
      [ScanCode.DOWN, StandardKey.DOWN],
      [ScanCode.LEFT, StandardKey.LEFT]
    ])

    this[$mappings] = new Map([
      [keyboardUuid, keyboardMapping]
    ])
  }

  get keyboard () {
    return this[$adapter].getKeyboard()
  }

  get gamepads () {
    return this[$adapter].getGamepads()
  }

  addGameControllerMappings (csv) {
    const adapter = this[$adapter]

    if (!adapter.addGameControllerMappings) {
      throw Error('addGameControllerMappings() is unavailable')
    }

    // TODO: update key mappings

    return adapter.addGameControllerMappings(csv)
  }

  [$bind] (stage) {
    const adapter = this[$adapter] = stage[$adapter]

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

      const mapping = this[$mappings].get(keyboard.uuid)

      if (!mapping) {
        console.log(`no mapping for: ${keyboard.uuid}`)
        return
      }

      if (!mapping.has(button)) {
        console.log(`unmapped keyboard button: ${button}`)
        return
      }

      const keyEvent = new KeyEvent(
        mapping.get(button),
        false,
        false,
        'standard',
        { device: keyboard, button: button },
        timestamp)

      stage[$scene][$bubble](keyEvent)
    })

    adapter.setCallback('keydown', (keyboard, button, repeat) => {
      const timestamp = now()
      const hardwareEvent = new DeviceButtonEvent(keyboard, button, true, repeat, timestamp)

      stage[$scene][$bubble](hardwareEvent)

      const mapping = this[$mappings].get(keyboard.uuid)

      if (!mapping) {
        console.log(`no mapping for: ${keyboard.uuid}`)
        return
      }

      if (!mapping.has(button)) {
        console.log(`unmapped keyboard button: ${button}`)
        return
      }

      const keyEvent = new KeyEvent(
        mapping.get(button),
        true,
        repeat,
        'standard',
        { device: keyboard, button: button },
        timestamp)

      stage[$scene][$capture](keyEvent)

      stage[$scene][$bubble](keyEvent)
    })

    adapter.setCallback('buttonup', (gamepad, button) => {
      const timestamp = now()
      const hardwareEvent = new DeviceButtonEvent(gamepad, button, false, false, timestamp)

      stage[$scene][$bubble](hardwareEvent)

      const mapping = this[$mappings].get(gamepad.uuid)

      if (!mapping) {
        console.log(`no mapping for: ${gamepad.uuid}`)
        return
      }

      if (!mapping.has(button)) {
        console.log(`unmapped keyboard button: ${button}`)
        return
      }

      const keyEvent = new KeyEvent(
        this[mapping].get(button),
        false,
        false,
        'standard',
        { device: gamepad, button: button },
        timestamp)

      stage[$scene][$bubble](keyEvent)
    })

    adapter.setCallback('buttondown', (gamepad, button) => {
      const timestamp = now()
      const hardwareEvent = new DeviceButtonEvent(gamepad, button, true, false, timestamp)

      stage[$scene][$bubble](hardwareEvent)

      const mapping = this[$mappings].get(gamepad.uuid)

      if (!mapping) {
        console.log(`no mapping for: ${gamepad.uuid}`)
        return
      }

      if (!mapping.has(button)) {
        console.log(`unmapped keyboard button: ${button}`)
        return
      }

      const keyEvent = new KeyEvent(
        this[mapping].get(button),
        true,
        false,
        'standard',
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

  [$unbind] () {
    // TODO: clear callbacks?
    this[$adapter] = unintializedStageAdapter
  }
}

const throwStageAdapterNotInitialized = () => {
  throw Error('stage.init() must be called before accessing input')
}

const unintializedStageAdapter = {
  addGameControllerMappings (csv) {
    throwStageAdapterNotInitialized()
  },
  get keyboard () {
    throwStageAdapterNotInitialized()
  },
  get gamepads () {
    throwStageAdapterNotInitialized()
  }
}

// const updateMapping = (device, mappings) => {
//   const { uuid, mapping } = device
//
//   !mappings.has(uuid) && mapping && mappings.set(uuid, mapping)
// }
