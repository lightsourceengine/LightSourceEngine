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

import { checkInstanceOfMapping, Mapping } from './Mapping.mjs'
import { Key } from './Key.mjs'
import { AnalogKey } from './AnalogKey.mjs'
import { ScanCode } from './ScanCode.mjs'
import { kKeyboardUUID } from './InputCommon.mjs'
import { InputDevice } from './InputDevice.mjs'

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
 *
 * @memberof module:@lse/core
 * @extends module:@lse/core.InputDevice
 * @hideconstructor
 */
class Keyboard extends InputDevice {
  _api
  _mapping

  constructor () {
    super({ id: 0, type: 'keyboard', uuid: kKeyboardUUID })
    this.$setApi(null)
    this.$setMapping(kDefaultKeyboardMapping)
    this.$connected = true
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
    return this._api.getScanCodeState(scanCode)
  }

  /**
   * @override
   */
  isKeyDown (key) {
    return this.getScanCodeState(this._mapping.getValue(key).button)
  }

  /**
   * @override
   */
  getAnalogValue (analog) {
    return this.getScanCodeState(this._mapping.getValue(analog).button) ? kAnalogMaxValue.get(analog) : 0
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
    this._name = this._mapping.name
  }

  /**
   * @ignore
   */
  $setApi (api) {
    this._api = api || { getScanCodeState (value) { return false } }
  }
}

const checkKeyboardUUID = (uuid) => {
  if (uuid !== kKeyboardUUID) {
    throw Error('Expected mapping to have the system Keyboard UUID: ' + kKeyboardUUID)
  }
}

export { Keyboard }
