/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { checkInstanceOfMapping, Mapping } from './Mapping.js'
import { Key } from './Key.js'
import { AnalogKey } from './AnalogKey.js'
import { ScanCode } from './ScanCode.js'
import { kKeyboardUUID } from './InputCommon.js'

const kDefaultKeyboardMapping = Mapping.forKeyboard([
  [Key.A, ScanCode.Z],
  [Key.B, ScanCode.X],
  [Key.X, ScanCode.A],
  [Key.Y, ScanCode.S],
  [Key.START, ScanCode.RETURN],
  [Key.BACK, ScanCode.RSHIFT],
  [Key.LEFT_SHOULDER, ScanCode.Q],
  [Key.RIGHT_SHOULDER, ScanCode.W],
  [Key.DPAD_UP, ScanCode.UP],
  [Key.DPAD_DOWN, ScanCode.DOWN],
  [Key.DPAD_LEFT, ScanCode.LEFT],
  [Key.DPAD_RIGHT, ScanCode.RIGHT]
])

const kAnalogMaxValue = new Map([
  [AnalogKey.LEFT_STICK_X, -1],
  [AnalogKey.RIGHT_STICK_X, -1],
  [AnalogKey.LEFT_TRIGGER, 1],
  [AnalogKey.RIGHT_TRIGGER, 1],
  [AnalogKey.LEFT_STICK_Y, 1],
  [AnalogKey.RIGHT_STICK_Y, 1]
])

/**
 * Keyboard input device.
 */
export class Keyboard {
  _nativeKeyboard = null
  _mapping = kDefaultKeyboardMapping

  /**
   * @returns {string} an instance type of 'keyboard'
   */
  get type () {
    return 'keyboard'
  }

  /**
   * @returns {number} the internal, unique id of this keyboard.
   */
  get id () {
    return 0
  }

  /**
   * @returns {string} the UUID or class of device hardware
   */
  get uuid () {
    return kKeyboardUUID
  }

  /**
   * The UI displayable keyboard name.
   *
   * The default name is 'System Keyboard', but the name can be overridden by mapping.
   *
   * @returns {string} the keyboard name
   */
  get name () {
    return this._mapping.name
  }

  /**
   * Get the state of a keyboard key by the hardware scan code id.
   *
   * The state of the keyboard is updated every frame.
   *
   * @param scanCode {ScanCode} Hardware keyboard key identifier.
   * @returns {boolean} state of the button; true for pressed, false for not-pressed or invalid scanCode
   */
  getScanCodeState (scanCode) {
    return this._nativeKeyboard?.isButtonDown(scanCode) ?? false
  }

  /**
   * Checks if a mapped key is currently pressed down.
   *
   * Note, the state of the keyboard is updated every frame. So, this method returns the state of key from the
   * last time input events were processed (either last frame or beginning of current frame).
   *
   * @param key {Key} Mapped key.
   * @returns {boolean} true if key is pressed; otherwise, false
   */
  isKeyDown (key) {
    return this.getScanCodeState(this._mapping.getValue(key).button)
  }

  /**
   * Gets the value of a mapped analog input, such as a control stick or trigger.
   *
   * Keyboards do not typically have analog inputs, but this method is provided so the keyboard can function with
   * the rest of the library as a gamepad.
   *
   * Note, the state of the keyboard is updated every frame. So, this method returns the state of analog input from the
   * last time input events were processed (either last frame or beginning of current frame).
   *
   * @param analog {AnalogKey} Analog input key.
   * @returns {number} value of the analog input
   */
  getAnalogValue (analog) {
    return this.getScanCodeState(this._mapping.getValue(analog).button) ? kAnalogMaxValue.get(analog) : 0
  }

  /**
   * @ignore
   */
  $setNative (nativeKeyboard) {
    this._nativeKeyboard = nativeKeyboard
  }

  /**
   * @ignore
   */
  $setMapping (mapping) {
    if (mapping) {
      checkInstanceOfMapping(mapping)
      checkKeyboardUUID(mapping.uuid)
      this._mapping = mapping
    } else {
      this._mapping = kDefaultKeyboardMapping
    }
  }
}

const checkKeyboardUUID = (uuid) => {
  if (uuid !== kKeyboardUUID) {
    throw Error('Expected mapping to have the system Keyboard UUID: ' + kKeyboardUUID)
  }
}
