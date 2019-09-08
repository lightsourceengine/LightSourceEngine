/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { performance } from 'perf_hooks'
import { Key } from './Key'
import { KeyEvent } from '../event/KeyEvent'
import { DeviceEvent } from '../event/DeviceEvent'
import { DeviceAxisEvent } from '../event/DeviceAxisEvent'
import { DeviceButtonEvent } from '../event/DeviceButtonEvent'
import {
  $attach,
  $detach,
  $emit,
  $init,
  $scene,
  $destroy,
  $setNativeKeyboard
} from '../util/InternalSymbols'
import { Mapping } from './Mapping'
import { Direction } from './Direction'
import { parseSystemMapping } from './parseSystemMapping'
import { DeviceHatEvent } from '../event/DeviceHatEvent'
import { Keyboard } from './Keyboard'
import { eventBubblePhase } from '../event/eventBubblePhase'
import { eventCapturePhase } from '../event/eventCapturePhase'

const { now } = performance

const emptyMapping = new Mapping([])

// InputManager private properties
const $stage = Symbol('stage')
const $stageAdapter = Symbol('stageAdapter')
const $systemMappings = Symbol('systemMappings')
const $userMappings = Symbol('nativeMappings')
const $gameControllerDb = Symbol('gameControllerDb')
const $keyToDirection = Symbol('keyToDirection')
const $enabled = Symbol('enabled')
const $attached = Symbol('attached')
const $keyboard = Symbol('keyboard')

/**
 *
 */
export class InputManager {
  /**
   *
   * @type {number}
   * @property lastActivity
   */
  lastActivity = now()

  constructor (stage) {
    this[$stage] = stage
    this[$enabled] = true
    this[$attached] = false
    this[$keyboard] = new Keyboard()

    // adapter passed in by stage on init
    this[$stageAdapter] = null

    // user defined map of device UUID to key Mapping
    this[$userMappings] = new Map()

    // system defined map of device UUID to key Mapping
    this[$systemMappings] = new Map()

    // conversion table for translating a mapped Key to a Direction. Each mapping name must have an entry.
    // note: mappingName -> key -> direction
    this[$keyToDirection] = new Map()

    // gamecontrollerdb.txt file name
    // note: only used if the stageAdapter supports Game Controller mappings
    this[$gameControllerDb] = null

    // add a keyToDirection entry for standard mapping
    this.registerMapping(Mapping.STANDARD, [
      [Key.UP, Direction.UP],
      [Key.RIGHT, Direction.RIGHT],
      [Key.DOWN, Direction.DOWN],
      [Key.LEFT, Direction.LEFT]])
  }

  /**
   *
   * @property enabled
   */
  get enabled () {
    return this[$enabled]
  }

  set enabled (value) {
    value = !!value

    if (value !== this[$enabled]) {
      if (this[$attached]) {
        if (value) {
          registerDeviceInputCallbacks(this)
        } else {
          this[$stageAdapter].resetCallbacks()
          registerDeviceConnectionCallbacks(this)
        }
      }

      this[$enabled] = value
    }
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
    return this[$stageAdapter] ? this[$stageAdapter].getGamepads() : []
  }

  /**
   *
   * @param uuid
   * @param keyMapping
   */
  setMapping (uuid, keyMapping) {
    if (!uuid || typeof uuid !== 'string') {
      throw Error(`uuid must be a string. Got: '${uuid}'`)
    }

    if (keyMapping !== null && !(keyMapping instanceof Mapping)) {
      throw Error('keyMapping must be a Mapping instance or null')
    }

    this[$userMappings].set(uuid, keyMapping)
  }

  /**
   *
   * @param uuid
   * @returns {*}
   */
  getMapping (uuid) {
    return this[$userMappings].get(uuid) || null
  }

  /**
   *
   * @param uuid
   */
  removeMapping (uuid) {
    this[$userMappings].delete(uuid)
  }

  /**
   *
   * @param uuid
   * @returns {*}
   */
  resolveMapping (uuid) {
    return this[$userMappings].has(uuid) ? this[$userMappings].get(uuid) : this[$systemMappings].get(uuid)
  }

  /**
   *
   * @param mappingName
   * @param keyToDirectionEntries
   */
  registerMapping (mappingName, keyToDirectionEntries) {
    const keyToDirectionMap = this[$keyToDirection]

    if (!mappingName || typeof mappingName !== 'string') {
      throw Error()
    }

    keyToDirectionMap.set(mappingName, new Map(keyToDirectionEntries))
  }

  [$init] (stageAdapter, gameControllerDb) {
    this[$gameControllerDb] = (gameControllerDb && typeof gameControllerDb === 'string') ? gameControllerDb : null
    this[$stageAdapter] = stageAdapter
    this[$attach]()
  }

  [$destroy] () {
    this[$detach]()
  }

  [$attach] () {
    if (this[$attached]) {
      return
    }

    const stageAdapter = this[$stageAdapter]
    const systemMappings = loadSystemMappings(stageAdapter, this[$gameControllerDb])

    this[$systemMappings] = systemMappings
    updateSystemMapping(systemMappings, this[$keyboard])
    this.gamepads.forEach((gamepad) => updateSystemMapping(systemMappings, gamepad))

    registerDeviceConnectionCallbacks(this)

    if (this[$enabled]) {
      registerDeviceInputCallbacks(this)
    }

    this[$keyboard][$setNativeKeyboard](stageAdapter.getKeyboard())
    this[$attached] = true
  }

  [$detach] () {
    if (!this[$attached]) {
      return
    }

    this[$keyboard][$setNativeKeyboard](null)
    this[$stageAdapter].resetCallbacks()
    this[$attached] = false
  }
}

const loadSystemMappings = (stageAdapter, gameControllerDb) => {
  const systemMappings = new Map()

  if (gameControllerDb && stageAdapter.addGameControllerMappings) {
    try {
      stageAdapter.addGameControllerMappings(gameControllerDb)
    } catch (e) {
      console.log(`Failed to load ${gameControllerDb}: ${e.message}`)
    }
  }

  return systemMappings
}

const updateSystemMapping = (systemMappings, { uuid, mapping }) => {
  if (mapping) {
    if (systemMappings.has(uuid)) {
      return
    }

    try {
      const [systemUUID, systemMapping] = parseSystemMapping(mapping)

      if (uuid === systemUUID) {
        systemMappings.set(uuid, systemMapping)
        return
      }

      console.log(`System mapping uuid mismatch: device=${uuid} system=${systemUUID}`)
    } catch (e) {
      console.log(`Failure reading system mapping: ${mapping}. Error: ${e.message}`)
    }
  }

  systemMappings.delete(uuid)
}

const createKeyEvent = (inputManager, key, pressed, repeat, mapping, source) => (
  new KeyEvent(
    key,
    pressed,
    repeat,
    mapping.name,
    inputManager[$keyToDirection].get(mapping.name).get(key) || Direction.NONE,
    source,
    source.timestamp)
)

const registerDeviceConnectionCallbacks = (inputManager) => {
  const stage = inputManager[$stage]
  const stageAdapter = inputManager[$stageAdapter]
  const systemMappings = inputManager[$systemMappings]

  stageAdapter.setCallback('connected', (gamepad) => {
    updateSystemMapping(systemMappings, gamepad)
    if (inputManager[$enabled]) {
      stage[$emit](new DeviceEvent(gamepad, true, now()))
    }
  })

  stageAdapter.setCallback('disconnected', (gamepad) => {
    if (inputManager[$enabled]) {
      stage[$emit](new DeviceEvent(gamepad, false, now()))
    }
  })
}

const registerDeviceInputCallbacks = (inputManager) => {
  const stage = inputManager[$stage]
  const stageAdapter = inputManager[$stageAdapter]
  const buttonUp = (device, button) => {
    if (device.type === Keyboard.TYPE) {
      device = inputManager.keyboard
    }

    const timestamp = now()
    const deviceEvent = new DeviceButtonEvent(device, button, false, false, timestamp)

    inputManager.lastActivity = timestamp
    eventBubblePhase(stage, stage[$scene], deviceEvent)

    const mapping = inputManager.resolveMapping(device.uuid) || emptyMapping
    const key = mapping.getKeyForButton(button)

    if (key >= 0) {
      eventBubblePhase(stage, stage[$scene], createKeyEvent(inputManager, key, false, false, mapping, deviceEvent))
    }
  }
  const buttonDown = (device, button, repeat) => {
    if (device.type === Keyboard.TYPE) {
      device = inputManager.keyboard
    }

    const timestamp = now()
    const deviceEvent = new DeviceButtonEvent(device, button, true, repeat, timestamp)

    inputManager.lastActivity = timestamp
    eventBubblePhase(stage, stage[$scene], deviceEvent)

    const mapping = inputManager.resolveMapping(device.uuid) || emptyMapping
    const key = mapping.getKeyForButton(button)

    if (key >= 0) {
      const keyEvent = createKeyEvent(inputManager, key, true, repeat, mapping, deviceEvent)

      eventCapturePhase(stage, stage[$scene], keyEvent)
      eventBubblePhase(stage, stage[$scene], keyEvent)
    }
  }
  const hatUp = (device, hat, value) => {
    const timestamp = now()
    const deviceEvent = new DeviceHatEvent(device, hat, value, false, false, timestamp)

    inputManager.lastActivity = timestamp
    eventBubblePhase(stage, stage[$scene], deviceEvent)

    const mapping = inputManager.resolveMapping(device.uuid) || emptyMapping
    const key = mapping.getKeyForHat(hat, value)

    if (key >= 0) {
      eventBubblePhase(stage, stage[$scene], createKeyEvent(inputManager, key, false, false, mapping, deviceEvent))
    }
  }
  const hatDown = (device, hat, value, repeat) => {
    const timestamp = now()
    const deviceEvent = new DeviceHatEvent(device, hat, value, true, repeat, timestamp)

    inputManager.lastActivity = timestamp
    eventBubblePhase(stage, stage[$scene], deviceEvent)

    const mapping = inputManager.resolveMapping(device.uuid) || emptyMapping
    const key = mapping.getKeyForHat(hat, value)

    if (key >= 0) {
      const keyEvent = createKeyEvent(inputManager, key, true, repeat, mapping, deviceEvent)

      eventCapturePhase(stage, stage[$scene], keyEvent)
      eventBubblePhase(stage, stage[$scene], keyEvent)
    }
  }
  const axisMotion = (device, axis, value) => {
    const timestamp = now()
    const hardwareEvent = new DeviceAxisEvent(device, axis, value, timestamp)

    inputManager.lastActivity = timestamp
    eventBubblePhase(stage, stage[$scene], hardwareEvent)

    // TODO: map axis to buttons
  }

  // Keyboard Key Callbacks

  stageAdapter.setCallback('keyup', buttonUp)
  stageAdapter.setCallback('keydown', buttonDown)

  // Joystick Button Callbacks

  stageAdapter.setCallback('buttonup', buttonUp)
  stageAdapter.setCallback('buttondown', buttonDown)

  // Joystick Hat Callbacks

  stageAdapter.setCallback('hatup', hatUp)
  stageAdapter.setCallback('hatdown', hatDown)

  // Joystick Axis Motion Callbacks

  stageAdapter.setCallback('axismotion', axisMotion)
}
