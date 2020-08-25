/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { performance } from 'perf_hooks'
import { Key } from './Key'
import { KeyEvent } from '../event/KeyEvent'
import { DeviceAxisEvent } from '../event/DeviceAxisEvent'
import { DeviceButtonEvent } from '../event/DeviceButtonEvent'
import { Mapping } from './Mapping'
import { Direction } from './Direction'
import { parseSystemMapping } from './parseSystemMapping'
import { DeviceHatEvent } from '../event/DeviceHatEvent'
import { Keyboard } from './Keyboard'
import { eventBubblePhase } from '../event/eventBubblePhase'
import { eventCapturePhase } from '../event/eventCapturePhase'
import { MappingType } from './MappingType'
import { emptyArray } from '../util'
import { InputDeviceType } from './InputDeviceType'

const { now } = performance

const emptyMapping = new Mapping([])

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

  _stage = null
  _isEnabled = true
  _isAttached = false
  _keyboard = new Keyboard()
  _plugin = null
  _userMappings = new Map()
  _systemMappings = new Map()
  _keyToDirection = new Map()
  _gameControllerDbFile = null

  constructor (stage) {
    this._stage = stage

    // add a keyToDirection entry for standard mapping
    this.registerMapping(MappingType.Standard, [
      [Key.UP, Direction.UP],
      [Key.RIGHT, Direction.RIGHT],
      [Key.DOWN, Direction.DOWN],
      [Key.LEFT, Direction.LEFT]
    ])
  }

  isEnabled () {
    return this._isEnabled
  }

  setEnabled (value) {
    value = !!value

    if (value !== this._isEnabled) {
      if (this._isAttached) {
        if (value) {
          this._registerDeviceInputCallbacks()
        } else {
          this._plugin.resetCallbacks()
          this._registerDeviceConnectionCallbacks()
        }
      }

      this._isEnabled = value
    }
  }

  /**
   *
   * @returns {Keyboard}
   */
  get keyboard () {
    return this._keyboard
  }

  /**
   *
   * @returns {Gamepad[]}
   */
  get gamepads () {
    return this._plugin ? this._plugin.getGamepads() : emptyArray
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

    this._userMappings.set(uuid, keyMapping)
  }

  /**
   *
   * @param uuid
   * @returns {*}
   */
  getMapping (uuid) {
    return this._userMappings.get(uuid) || null
  }

  /**
   *
   * @param uuid
   */
  removeMapping (uuid) {
    this._userMappings.delete(uuid)
  }

  /**
   *
   * @param uuid
   * @returns {*}
   */
  resolveMapping (uuid) {
    return this._userMappings.has(uuid) ? this._userMappings.get(uuid) : this._systemMappings.get(uuid)
  }

  /**
   *
   * @param mappingName
   * @param keyToDirectionEntries
   */
  registerMapping (mappingName, keyToDirectionEntries) {
    const keyToDirectionMap = this._keyToDirection

    if (!mappingName || typeof mappingName !== 'string') {
      throw Error()
    }

    keyToDirectionMap.set(mappingName, new Map(keyToDirectionEntries))
  }

  /**
   * @ignore
   */
  $setPlugin (plugin) {
    this._plugin = plugin
  }

  /**
   * @ignore
   */
  $attach () {
    if (this._isAttached) {
      return
    }

    const plugin = this._plugin
    const systemMappings = loadSystemMappings(plugin, this._gameControllerDbFile)

    this._systemMappings = systemMappings
    updateSystemMapping(systemMappings, this._keyboard)
    this.gamepads.forEach((gamepad) => updateSystemMapping(systemMappings, gamepad))

    this._registerDeviceConnectionCallbacks()

    if (this._isEnabled) {
      this._registerDeviceInputCallbacks()
    }

    this._keyboard.$setNative(plugin.getKeyboard())
    this._isAttached = true
  }

  /**
   * @ignore
   */
  $detach () {
    if (!this._isAttached) {
      return
    }

    this._keyboard.$setNative(null)
    this._plugin.resetCallbacks()
    this._isAttached = false
  }

  /**
   * @ignore
   */
  $destroy () {
    // TODO: don't call detach
    this.$detach()
  }

  /**
   * @ignore
   */
  // Test API. May need to promote this to public, as it may be useful.
  $sendKeyEvent (key, pressed, repeat, mapping) {
    if (key >= 0 && this._keyToDirection.has(mapping)) {
      const keyEvent = this._createKeyEvent(key, true, repeat, mapping, { timestamp: now() })
      const stage = this._stage

      eventCapturePhase(stage, stage.getScene(), keyEvent)
      eventBubblePhase(stage, stage.getScene(), keyEvent)
    }
  }

  /**
   * @ignore
   */
  _createKeyEvent (key, pressed, repeat, mappingName, source) {
    return new KeyEvent(
      key,
      pressed,
      repeat,
      mappingName,
      this._keyToDirection.get(mappingName).get(key) || Direction.NONE,
      source,
      source.timestamp
    )
  }

  /**
   * @ignore
   */
  _registerDeviceConnectionCallbacks () {
    const plugin = this._plugin
    const systemMappings = this._systemMappings

    plugin.setCallback('connected', (gamepad) => {
      updateSystemMapping(systemMappings, gamepad)
      if (this._isEnabled) {
        // TODO: stage.$emit(new DeviceEvent(gamepad, true, now()))
      }
    })

    plugin.setCallback('disconnected', (gamepad) => {
      if (this._isEnabled) {
        // TODO: stage.$emit(new DeviceEvent(gamepad, false, now()))
      }
    })
  }

  /**
   * @ignore
   */
  _registerDeviceInputCallbacks () {
    const stage = this._stage
    const plugin = this._plugin
    const buttonUp = (device, button) => {
      if (device.type === InputDeviceType.Keyboard) {
        device = this.keyboard
      }

      const timestamp = now()
      const deviceEvent = new DeviceButtonEvent(device, button, false, false, timestamp)

      this.lastActivity = timestamp
      eventBubblePhase(stage, stage.getScene(), deviceEvent)

      const mapping = this.resolveMapping(device.uuid) || emptyMapping
      const key = mapping.getKeyForButton(button)

      if (key >= 0) {
        eventBubblePhase(stage, stage.getScene(), this._createKeyEvent(key, false, false, mapping.name, deviceEvent))
      }
    }
    const buttonDown = (device, button, repeat) => {
      if (device.type === InputDeviceType.Keyboard) {
        device = this.keyboard
      }

      const timestamp = now()
      const deviceEvent = new DeviceButtonEvent(device, button, true, repeat, timestamp)

      this.lastActivity = timestamp
      eventBubblePhase(stage, stage.getScene(), deviceEvent)

      const mapping = this.resolveMapping(device.uuid) || emptyMapping
      const key = mapping.getKeyForButton(button)

      if (key >= 0) {
        const keyEvent = this._createKeyEvent(key, true, repeat, mapping.name, deviceEvent)

        eventCapturePhase(stage, stage.getScene(), keyEvent)
        eventBubblePhase(stage, stage.getScene(), keyEvent)
      }
    }
    const hatUp = (device, hat, value) => {
      const timestamp = now()
      const deviceEvent = new DeviceHatEvent(device, hat, value, false, false, timestamp)

      this.lastActivity = timestamp
      eventBubblePhase(stage, stage.getScene(), deviceEvent)

      const mapping = this.resolveMapping(device.uuid) || emptyMapping
      const key = mapping.getKeyForHat(hat, value)

      if (key >= 0) {
        eventBubblePhase(stage, stage.getScene(), this._createKeyEvent(key, false, false, mapping.name, deviceEvent))
      }
    }
    const hatDown = (device, hat, value, repeat) => {
      const timestamp = now()
      const deviceEvent = new DeviceHatEvent(device, hat, value, true, repeat, timestamp)

      this.lastActivity = timestamp
      eventBubblePhase(stage, stage.getScene(), deviceEvent)

      const mapping = this.resolveMapping(device.uuid) || emptyMapping
      const key = mapping.getKeyForHat(hat, value)

      if (key >= 0) {
        const keyEvent = this._createKeyEvent(key, true, repeat, mapping.name, deviceEvent)

        eventCapturePhase(stage, stage.getScene(), keyEvent)
        eventBubblePhase(stage, stage.getScene(), keyEvent)
      }
    }
    const axisMotion = (device, axis, value) => {
      const timestamp = now()
      const hardwareEvent = new DeviceAxisEvent(device, axis, value, timestamp)

      this.lastActivity = timestamp
      eventBubblePhase(stage, stage.getScene(), hardwareEvent)

      // TODO: map axis to buttons
    }

    // Keyboard Key Callbacks

    plugin.setCallback('keyup', buttonUp)
    plugin.setCallback('keydown', buttonDown)

    // Joystick Button Callbacks

    plugin.setCallback('buttonup', buttonUp)
    plugin.setCallback('buttondown', buttonDown)

    // Joystick Hat Callbacks

    plugin.setCallback('hatup', hatUp)
    plugin.setCallback('hatdown', hatDown)

    // Joystick Axis Motion Callbacks

    plugin.setCallback('axismotion', axisMotion)
  }
}

/**
 * @ignore
 */
const loadSystemMappings = (plugin, gameControllerDb) => {
  const systemMappings = new Map()

  if (gameControllerDb && plugin.addGameControllerMappings) {
    try {
      plugin.addGameControllerMappings(gameControllerDb)
    } catch (e) {
      console.log(`Failed to load ${gameControllerDb}: ${e.message}`)
    }
  }

  return systemMappings
}

/**
 * @ignore
 */
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
