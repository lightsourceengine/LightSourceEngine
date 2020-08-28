/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ScanCode } from './ScanCode'
import { InputDeviceType } from './InputDeviceType'

/**
 *
 */
export class Keyboard {
  static UUID = '00000000-00000000-00000000-00000001'
  static Name = 'SystemKeyboard'

  _nativeKeyboard = null

  /**
   *
   * @returns {string}
   */
  get type () {
    return InputDeviceType.Keyboard
  }

  /**
   *
   * @returns {number}
   */
  get id () {
    return 0
  }

  /**
   *
   * @returns {string}
   */
  get uuid () {
    return Keyboard.UUID
  }

  /**
   *
   * @returns {string}
   */
  get name () {
    return Keyboard.Name
  }

  /**
   *
   * @param scanCode
   * @returns {boolean}
   */
  isButtonDown (scanCode) {
    return this._nativeKeyboard ? this._nativeKeyboard.isButtonDown(scanCode) : false
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
  $getGameControllerMapping () {
    return kKeyboardGameControllerCsv
  }
}

const kKeyboardGameControllerCsv =
  `${Keyboard.UUID},${Keyboard.Name},b:b${ScanCode.Z},a:b${ScanCode.X},x:b${ScanCode.S},y:b${ScanCode.A},\
start:b${ScanCode.RETURN},back:b${ScanCode.RSHIFT},leftshoulder:b${ScanCode.Q},rightshoulder:b${ScanCode.W},\
dpup:b${ScanCode.UP},dpdown:b${ScanCode.DOWN},dpleft:b${ScanCode.LEFT},dpright:b${ScanCode.RIGHT},platform:Linux,`
