/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Key } from './Key.js'
import { checkInstanceOfMapping, createEmptyGameControllerCsv } from './Mapping.js'
import { Direction } from './Direction.js'
import { Keyboard } from './Keyboard.js'
import { Gamepad } from './Gamepad.js'
import { eventCapturePhase } from '../event/eventCapturePhase.js'
import { now } from '../util/index.js'
import { promises } from 'fs'
import {
  createAttachedEvent,
  createDetachedEvent,
  createAnalogMotionEvent,
  createGamepadConnectedEvent,
  createGamepadDisconnectedEvent,
  createKeyDownEvent,
  createKeyUpEvent,
  createAxisMotionEvent,
  createHatMotionEvent,
  createScanCodeDownEvent,
  createScanCodeUpEvent,
  createButtonDownEvent,
  createButtonUpEvent
} from '../event/index.js'
import { EventName } from '../event/EventName.js'
import { kKeyboardUUID } from './InputCommon.js'
import { logger } from '../addon/index.js'
import { EventTarget } from '../event/EventTarget.js'

const kOnKeyUp = 'onKeyUp'
const kOnKeyDown = 'onKeyDown'

export class InputManager extends EventTarget {
  _stage = null
  _isEnabled = true
  _isAttached = false
  _keyboard = new Keyboard()
  _gamepads = new Map()
  _plugin = null
  _gamepadMappings = new Map()
  _navigationMapping = new Map()
  _loadIntrinsicMappingDbTicket = 0
  _intrinsicMappingDb = ''

  lastActivity = now()

  constructor (stage) {
    super([
      EventName.onAttached,
      EventName.onDetached,
      EventName.onConnected,
      EventName.onDisconnected,
      EventName.onScanCodeUp,
      EventName.onScanCodeDown,
      EventName.onButtonUp,
      EventName.onButtonDown,
      EventName.onAxisMotion,
      EventName.onHatMotion,
      EventName.onKeyUp,
      EventName.onKeyDown,
      EventName.onAnalogMotion
    ])

    this._stage = stage
    this.resetNavigationMapping()
    this.loadIntrinsicMappingDb({ file: process.env.LSE_GAME_CONTROLLER_DB })
  }

  isEnabled () {
    return this._isEnabled
  }

  setEnabled (value) {
    value = !!value

    if (value === this._isEnabled) {
      return
    }

    if (this._isAttached) {
      if (value) {
        this._registerCallbacks()
      } else {
        this._plugin.resetCallbacks()
      }
    }

    this._isEnabled = value
  }

  /**
   * Get the system keyboard.
   *
   * The keyboard is available when the input manager is detached.
   *
   * @returns {Keyboard}
   */
  get keyboard () {
    return this._keyboard
  }

  /**
   * Get the list of gamepads connected to this device.
   *
   * When the input manager is detached, the returned list will be empty.
   *
   * @returns {Gamepad[]}
   */
  get gamepads () {
    return Array.from(this._gamepads.values())
  }

  /**
   * Set a user mapping for a device.
   *
   * @param mapping {Mapping} mapping to set
   */
  setMapping (mapping) {
    checkInstanceOfMapping(mapping)

    const { uuid } = mapping

    if (uuid === kKeyboardUUID) {
      this._keyboard.$setMapping(mapping)

      return
    }

    if (this._gamepadMappings.has(uuid)) {
      this._gamepadMappings.get(uuid).userMapping = mapping
    } else {
      this._gamepadMappings.set(uuid, {
        userMapping: mapping,
        intrinsicMapping: this._isAttached ? this._plugin.getGameControllerMapping(uuid) : null
      })
    }

    if (this._isAttached) {
      this._plugin.loadGameControllerMappings(mapping.toCsv())
    }
  }

  /**
   * Get a user mapping for a device.
   *
   * @param uuid {string} Device UUID
   * @returns {Mapping} mapping instance for the device. if no device exists with uuid, null is returned
   */
  getMapping (uuid) {
    if (uuid === kKeyboardUUID) {
      return this._keyboard._mapping
    }

    return this._gamepadMappings.get(uuid)?.userMapping ?? null
  }

  /**
   * Remove user mapping associated with a device.
   *
   * After the user mapping has been removed, the default mapping for the device will be restored.
   *
   * @param uuid {string} device UUID
   */
  removeMapping (uuid) {
    if (uuid === kKeyboardUUID) {
      this._keyboard.$setMapping(null)
    } else if (this._gamepadMappings.has(uuid) && this._isAttached) {
      this._plugin.loadGameControllerMappings(
        this._gamepadMappings.get(uuid)?.intrinsicMapping ?? createEmptyGameControllerCsv(uuid))
    }

    this._gamepadMappings.delete(uuid)
  }

  /**
   * Checks if a device has a user mapping OR intrinsic mapping.
   *
   * @param uuid {string} device UUID
   * @returns {boolean} true, device is mapping; false, otherwise
   */
  hasMapping (uuid) {
    if (uuid === kKeyboardUUID || this._gamepadMappings.has(uuid)) {
      return true
    } else if (this._isAttached) {
      return !!this._plugin.getGameControllerMapping(uuid)
    }

    return false
  }

  /**
   * Set the spacial navigation mappings.
   *
   * Maps Key enums to Direction enums.
   *
   * @param entries {array} Array of key-value pairs (same format as Map constructor)
   *
   * @example
   * // entries format
   * [
   *   [Key, Direction],
   *   ...
   * ])
   */
  setNavigationMapping (entries) {
    if (!Array.isArray(entries)) {
      throw Error('entries must be an array of [Key, Direction] pairs.')
    }
    this._navigationMapping = new Map(entries)
  }

  /**
   * Reset to the default spacial navigation mappings.
   *
   * @example
   * // Default mappings
   * [
   *   [Key.DPAD_UP, Direction.UP],
   *   [Key.DPAD_RIGHT, Direction.RIGHT],
   *   [Key.DPAD_DOWN, Direction.DOWN],
   *   [Key.DPAD_LEFT, Direction.LEFT]
   * ])
   */
  resetNavigationMapping () {
    this.setNavigationMapping([
      [Key.DPAD_UP, Direction.UP],
      [Key.DPAD_RIGHT, Direction.RIGHT],
      [Key.DPAD_DOWN, Direction.DOWN],
      [Key.DPAD_LEFT, Direction.LEFT]
    ])
  }

  /**
   * @ignore
   */
  loadIntrinsicMappingDb (options) {
    const ticket = ++this._loadIntrinsicMappingDbTicket
    const commitMappingDb = (db) => {
      this._intrinsicMappingDb = db
      if (this._isAttached) {
        this._plugin.loadGameControllerMappings(this._intrinsicMappingDb)
      }
    }

    if (typeof options === 'string') {
      options = { file: options }
    }

    const { file, csv } = options

    if (typeof file === 'string') {
      promises.readFile(file)
        .then(contents => {
          if (ticket === this._loadIntrinsicMappingDbTicket) {
            commitMappingDb(contents)
          }
        })
        .catch(e => {
          if (ticket === this._loadIntrinsicMappingDbTicket) {
            logger.warn(`Failed to load intrinsic mapping file: ${file}`)
          }
        })
    } else if (typeof csv === 'string') {
      commitMappingDb(Buffer.from(csv, 'utf8'))
    } else if (Buffer.isBuffer(csv)) {
      commitMappingDb(csv)
    }
  }

  /**
   * @ignore
   */
  $setPlugin (plugin) {
    this._plugin = plugin
    this._keyboard.$setApi(plugin)
  }

  /**
   * @ignore
   */
  $attach () {
    if (this._isAttached) {
      return
    }

    const { _plugin, _intrinsicMappingDb, _gamepadMappings, _gamepads, _isEnabled } = this

    if (_intrinsicMappingDb) {
      _plugin.loadGameControllerMappings(_intrinsicMappingDb)
    }

    _gamepadMappings.forEach(({ intrinsicMapping }) => _plugin.loadGameControllerMappings(intrinsicMapping))

    _gamepads.clear()

    for (const id of _plugin.getGamepadInstanceIds()) {
      _gamepads.set(id, new Gamepad(id, _plugin))
    }

    if (_isEnabled) {
      this._registerCallbacks()
    }

    this.lastActivity = now()
    this._isAttached = true

    this.dispatchEvent(createAttachedEvent(this))
  }

  /**
   * @ignore
   */
  $detach () {
    if (!this._isAttached) {
      return
    }

    const { _keyboard, _gamepads, _plugin } = this

    _keyboard.$setApi(null)
    _gamepads.forEach(gamepad => { gamepad.$connected = false })
    _gamepads.clear()
    _plugin.resetCallbacks()
    this._isAttached = false

    this.dispatchEvent(createDetachedEvent(this))
  }

  /**
   * @ignore
   */
  $destroy () {
    this.$detach()
  }

  /**
   * @ignore
   */
  _registerCallbacks () {
    const { _plugin } = this

    _plugin.onGamepadStatus = (id, connected) => {
      this.lastActivity = now()

      const { _gamepads } = this
      let Event
      let gamepad

      if (connected) {
        gamepad = new Gamepad(id, _plugin)
        _gamepads.set(id, gamepad)
        Event = createGamepadConnectedEvent
      } else if (_gamepads.has(id)) {
        gamepad = _gamepads.get(id)
        gamepad.$connected = false
        Event = createGamepadDisconnectedEvent
      }

      Event && this.dispatchEvent(Event(this, gamepad))
    }

    _plugin.onKeyboardScanCode = (button, pressed, repeat) => {
      this.lastActivity = now()
      const { _keyboard } = this

      this.dispatchEvent(pressed ? createScanCodeDownEvent(this, _keyboard, button, repeat) : createScanCodeUpEvent(this, _keyboard, button))

      const key = _keyboard._mapping.getKeyForButton(button)

      if (key >= 0) {
        if (pressed) {
          this._sendKeyDown(_keyboard, key, repeat)
        } else {
          this._sendKeyUp(_keyboard, key)
        }
      }
    }

    _plugin.onGamepadButton = (id, button, pressed) => {
      this.lastActivity = now()

      const gamepad = this._gamepads.get(id)

      this.dispatchEvent(pressed ? createButtonDownEvent(this, gamepad, button, false) : createButtonUpEvent(this, gamepad, button))
    }

    _plugin.onGamepadButtonMapped = (id, button, pressed) => {
      this.lastActivity = now()

      const gamepad = this._gamepads.get(id)

      pressed ? this._sendKeyDown(gamepad, button, false) : this._sendKeyUp(gamepad, button)
    }

    const createAxisLikeCallback = (eventCreator) => {
      const self = this
      const eventCreatorConst = eventCreator

      return (id, axisId, value) => {
        this.lastActivity = now()
        self.dispatchEvent(eventCreatorConst(self, self._gamepads.get(id), axisId, value))
      }
    }

    _plugin.onGamepadHat = createAxisLikeCallback(createHatMotionEvent)
    _plugin.onGamepadAxis = createAxisLikeCallback(createAxisMotionEvent)
    _plugin.onGamepadAxisMapped = createAxisLikeCallback(createAnalogMotionEvent)
  }

  /**
   * @ignore
   */
  _sendKeyUp (device, key) {
    // Phase: Dispatch to InputManager listeners

    const keyUp = createKeyUpEvent(this, key)

    this.dispatchEvent(keyUp)

    if (keyUp.hasStopPropagation()) {
      return
    }

    // Phase: Bubble

    const { activeNode } = this._stage.getScene(-1)

    activeNode?.$bubble(keyUp, kOnKeyUp)
  }

  /**
   * @ignore
   */
  _sendKeyDown (device, key, repeat) {
    // Phase: Dispatch to InputManager listeners

    const keyDown = createKeyDownEvent(this, key, repeat)

    this.dispatchEvent(keyDown)

    if (keyDown.hasStopPropagation()) {
      return
    }

    const { activeNode } = this._stage.getScene(-1)

    // Phase: Navigate

    eventCapturePhase(activeNode, this._navigationMapping.get(key) ?? Direction.NONE, keyDown)

    if (keyDown.hasStopPropagation()) {
      return
    }

    // Phase: Bubble

    activeNode?.$bubble(keyDown, kOnKeyDown)
  }
}
