/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { ScanCode } from './ScanCode'
import { $setNativeKeyboard } from '../util/InternalSymbols'

const $nativeKeyboard = Symbol('nativeKeyboard')
const keyboardId = 0
const keyboardUUID = 'keyboard'
const keyboardName = 'SystemKeyboard'
const keyboardType = 'keyboard'
const keyboardMappingString =
`${keyboardUUID},${keyboardName},b:b${ScanCode.Z},a:b${ScanCode.X},x:b${ScanCode.S},y:b${ScanCode.A},\
start:b${ScanCode.RETURN},back:b${ScanCode.RSHIFT},leftshoulder:b${ScanCode.Q},rightshoulder:b${ScanCode.W},\
dpup:b${ScanCode.UP},dpdown:b${ScanCode.DOWN},dpleft:b${ScanCode.LEFT},dpright:b${ScanCode.RIGHT},platform:Linux,`

/**
 *
 */
export class Keyboard {
  static TYPE = keyboardType

  constructor () {
    this[$nativeKeyboard] = null
  }

  /**
   *
   * @returns {string}
   */
  get type () {
    return keyboardType
  }

  /**
   *
   * @returns {number}
   */
  get id () {
    return keyboardId
  }

  /**
   *
   * @returns {string}
   */
  get uuid () {
    return keyboardUUID
  }

  /**
   *
   * @returns {string}
   */
  get name () {
    return keyboardName
  }

  /**
   *
   * @returns {string}
   */
  get mapping () {
    return keyboardMappingString
  }

  /**
   *
   * @param scanCode
   * @returns {boolean}
   */
  isButtonDown (scanCode) {
    return this[$nativeKeyboard] ? this[$nativeKeyboard].isButtonDown(scanCode) : false
  }

  [$setNativeKeyboard] (nativeKeyboard) {
    this[$nativeKeyboard] = nativeKeyboard
  }
}
