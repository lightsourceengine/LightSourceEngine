/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Key } from './Key.js'
import { checkInstanceOfMapping, createEmptyGameControllerCsv } from './Mapping.js'
import { Direction } from './Direction.js'
import { Keyboard } from './Keyboard.js'
import { eventCapturePhase } from '../event/eventCapturePhase.js'
import { emptyArray, EventEmitter, now } from '../util/index.js'
import { promises } from 'fs'
import {
  AttachedEvent,
  DetachedEvent,
  EventNames,
  GamepadConnectedEvent,
  GamepadDisconnectedEvent,
  KeyDownEvent,
  KeyUpEvent,
  AxisMotionEvent,
  RawAxisMotionEvent,
  RawHatMotionEvent,
  RawKeyDownEvent,
  RawKeyUpEvent
} from '../event/index.js'
import { kKeyboardUUID } from './InputCommon.js'

export class InputManager {
  _stage = null
  _isEnabled = true
  _isAttached = false
  _keyboard = new Keyboard()
  _plugin = null
  _gamepadMappings = new Map()
  _navigationMapping = new Map()
  _loadIntrinsicMappingDbTicket = 0
  _intrinsicMappingDb = ''
  _emitter = new EventEmitter([
    EventNames.attached,
    EventNames.detached,
    EventNames.connected,
    EventNames.disconnected,
    EventNames.rawkeyup,
    EventNames.rawkeydown,
    EventNames.rawaxismotion,
    EventNames.rawhatmotion,
    EventNames.keyup,
    EventNames.keydown,
    EventNames.axismotion
  ])

  lastActivity = now()

  constructor (stage) {
    this._stage = stage
    this.resetNavigationMapping()
  }

  on (id, listener) {
    this._emitter.on(id, listener)
  }

  once (id, listener) {
    this._emitter.once(id, listener)
  }

  off (id, listener) {
    this._emitter.off(id, listener)
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
    return this._plugin?.getGamepads() ?? emptyArray
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
      this._plugin.loadGameControllerMappings()
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

    if (typeof options === 'string') {
      options = { file: options }
    }

    if (typeof options.file === 'string') {
      promises.readFile(options.file)
        .then(contents => {
          if (ticket === this._loadIntrinsicMappingDbTicket) {
            this._intrinsicMappingDb = contents
            if (this._isAttached) {
              this._plugin.loadGameControllerMappings(this._intrinsicMappingDb)
            }
          }
        })
        .catch(e => {
          if (ticket === this._loadIntrinsicMappingDbTicket) {
            console.log('!!!!')
          }
        })
    } else if (typeof options.csv === 'string') {
      this._intrinsicMappingDb = Buffer.from(options.csv, 'utf8')

      if (this._isAttached) {
        this._plugin.loadGameControllerMappings(this._intrinsicMappingDb)
      }
    }
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

    if (this._intrinsicMappingDb) {
      plugin.loadGameControllerMappings(this._intrinsicMappingDb)
    }

    this._gamepadMappings.forEach(({ intrinsicMapping }) => plugin.loadGameControllerMappings(intrinsicMapping))

    if (this._isEnabled) {
      this._registerCallbacks()
    } else {
      plugin.resetCallbacks()
    }

    this._keyboard.$setNative(plugin.getKeyboard())
    this.lastActivity = now()
    this._isAttached = true

    this._emitter.emitEvent(AttachedEvent(this))
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

    this._emitter.emitEvent(DetachedEvent(this))
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
  _registerCallbacks () {
    const plugin = this._plugin

    plugin.setCallback('gamepad:status', (gamepad, connected) => {
      this.lastActivity = now()

      const eventType = connected ? GamepadConnectedEvent : GamepadDisconnectedEvent
      this._emitter.emitEvent(eventType(this, gamepad))
    })

    plugin.setCallback('gamepad:button-mapped', (device, button, pressed) => {
      this.lastActivity = now()

      if (pressed) {
        this._sendKeyDown(device, button, false)
      } else {
        this._sendKeyUp(device, button)
      }
    })

    plugin.setCallback('gamepad:axis-mapped', (device, axis, value) => {
      this.lastActivity = now()
      this._emitter.emitEvent(AxisMotionEvent(device, axis, value))
    })

    plugin.setCallback('keyboard:button', (device, button, pressed, repeat) => {
      this.lastActivity = now()
      device = this._keyboard

      this._emitter.emitEvent(pressed ? RawKeyDownEvent(this, device, button, repeat) : RawKeyUpEvent(this, device, button))

      const key = this._keyboard._mapping.getKeyForButton(button)

      if (key >= 0) {
        if (pressed) {
          this._sendKeyDown(device, key, repeat)
        } else {
          this._sendKeyUp(device, key)
        }
      }
    })

    plugin.setCallback('gamepad:button', (device, button, pressed) => {
      this.lastActivity = now()
      this._emitter.emitEvent(pressed ? RawKeyDownEvent(this, device, button, false) : RawKeyUpEvent(this, device, button))
    })

    plugin.setCallback('gamepad:hat', (device, hat, value) => {
      this.lastActivity = now()
      this._emitter.emitEvent(RawHatMotionEvent(this, device, hat, value))
    })

    plugin.setCallback('gamepad:axis', (device, axis, value) => {
      this.lastActivity = now()
      this._emitter.emitEvent(RawAxisMotionEvent(this, device, axis, value))
    })
  }

  /**
   * @ignore
   */
  _sendKeyUp (device, key) {
    // Phase: Dispatch to InputManager listeners

    const keyUp = KeyUpEvent(this, key)

    this._emitter.emitEvent(KeyUpEvent(this, key))

    if (keyUp.hasStopPropagation()) {
      return
    }

    // Phase: Bubble

    const { activeNode } = this._stage.getScene()

    activeNode?.$bubble(keyUp, 'onKeyUp')
  }

  /**
   * @ignore
   */
  _sendKeyDown (device, key, repeat) {
    // Phase: Dispatch to InputManager listeners

    const keyDown = KeyDownEvent(this, key, repeat)

    this._emitter.emitEvent(KeyDownEvent(this, key, repeat))

    if (keyDown.hasStopPropagation()) {
      return
    }

    const { activeNode } = this._stage.getScene()

    // Phase: Navigate

    eventCapturePhase(activeNode, this._navigationMapping.get(key) ?? Direction.NONE, keyDown)

    if (keyDown.hasStopPropagation()) {
      return
    }

    // Phase: Bubble

    activeNode?.$bubble(keyDown, 'onKeyDown')
  }
}
