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

import { AnalogKey } from './AnalogKey.mjs'
import { Key } from './Key.mjs'
import { invertMap, isPlainObject } from '../util/index.mjs'
import { kDefaultKeyboardName, kKeyboardUUID } from './InputCommon.mjs'

class MappingValue {
  button
  hat
  axis
  meta

  isButton () {
    return typeof this.button === 'number'
  }

  isAxis () {
    return typeof this.axis === 'number'
  }

  isHat () {
    return typeof this.hat === 'number'
  }

  static forButton (button) {
    const value = new MappingValue()

    if (!Number.isInteger(button)) {
      throw Error('button must be an integer >= 0')
    }

    value.button = button

    return value
  }

  static forAxis (axis, meta) {
    const value = new MappingValue()

    if (!Number.isInteger(axis) || axis < 0) {
      throw Error('axis index must be an integer >= 0')
    }

    if (meta) {
      switch (meta) {
        case '+':
        case '-':
        case '~':
          break
        default:
          throw Error('axis meta must be a "+", "-", "~" or undefined')
      }
    } else {
      meta = ''
    }

    value.axis = axis
    value.meta = meta

    return value
  }

  static forHat (hat, meta) {
    const value = new MappingValue()

    if (!Number.isInteger(hat) || hat < 0) {
      throw Error('hat index must be an integer >= 0')
    }

    switch (meta) {
      case 1:
      case 2:
      case 4:
      case 8:
        break
      default:
        throw Error('hat modifier must be an integer value of 1, 2, 4 or 8')
    }

    value.hat = hat
    value.meta = meta

    return value
  }
}

/**
 * Handles the use case of the user setting custom controls for an input device, such as a keyboard or
 * gamepad.
 *
 * @memberof module:@lse/core
 * @hideconstructor
 */
class Mapping {
  static Value = MappingValue

  _keys = new Map()
  _buttonToKey = new Map()

  constructor (uuid, name, entries) {
    if (typeof uuid !== 'string' || uuid.length !== 32) {
      throw Error(`invalid uuid: ${uuid}`)
    }

    if (typeof name !== 'string' || name.length === 0) {
      throw Error('name must be a string')
    }

    this.uuid = uuid
    this.name = name

    for (let [key, hardwareId] of entries) {
      if (!Number.isInteger(key)) {
        throw Error('mapped key must be an integer. got: ' + key)
      }

      if (typeof hardwareId === 'string') {
        hardwareId = parseHardwareIdString(hardwareId)
      } else if (Number.isInteger(hardwareId)) {
        hardwareId = MappingValue.forButton(hardwareId)
      } else if (hardwareId instanceof MappingValue) {
        // continue
      } else if (isPlainObject(hardwareId)) {
        if ('button' in hardwareId) {
          hardwareId = MappingValue.forButton(hardwareId.button)
        } else if ('axis' in hardwareId) {
          hardwareId = MappingValue.forAxis(hardwareId.axis, hardwareId.meta)
        } else if ('hat' in hardwareId) {
          hardwareId = MappingValue.forHat(hardwareId.hat, hardwareId.meta)
        } else {
          throw Error('No button, axis or hat value set for hardware id')
        }
      } else {
        throw Error('Invalid hardware id: ' + hardwareId)
      }

      if (hardwareId.isButton()) {
        this._buttonToKey.set(hardwareId.button, key)
      }

      this._keys.set(key, hardwareId)
    }
  }

  /**
   * Get a hardware input id for a mapped Key or AnalogKey.
   *
   * @param key {Key|AnalogKey} id to search on
   * @returns {MappingValue}
   */
  getValue (key) {
    return this._keys.get(key) ?? kEmptyMappingValue
  }

  /**
   * Find the mapped key for a hardware button id.
   *
   * @param buttonId {number} hardware button id
   * @returns {number} if the hardware button is mapped, a Key or AnalogKey value is returned. otherwise, -1.
   */
  getKeyForButton (buttonId) {
    return this._buttonToKey.get(buttonId) ?? -1
  }

  /**
   * Serialize this object as a CSV string.
   *
   * The CSV string is formatted in SDL GameController mapping format.
   *
   * @returns {string} CSV string
   */
  toCsv () {
    const components = [this.uuid, this.name]

    for (const [key, hwId] of this._keys) {
      let value

      if (hwId.isButton()) {
        value = `b${hwId.button}`
      } else if (hwId.isAxis()) {
        const isTilde = hwId.meta === '~'
        value = `${isTilde ? '' : hwId.meta}a${hwId.axis}${isTilde ? hwId.meta : ''}`
      } else if (hwId.isHat()) {
        value = `h${hwId.hat}.${hwId.meta}`
      }

      components.push(`${kKeyToGameControllerId.get(key)}:${value}`)
    }

    components.push(`platform:${kToGameControllerPlatform.get(process.platform) ?? '?'}`)
    components.push('')

    return components.join(',')
  }

  /**
   * Create a mapping for a keyboard input device.
   *
   * @param entries {array} List of Key/Analog and hardware id pairs
   * @param options object containing an optional display name for the device
   * @returns {Mapping}
   */
  static forKeyboard (entries, options = { name: kDefaultKeyboardName }) {
    const mapping = new Mapping(kKeyboardUUID, options.name, entries)

    mapping._keys.forEach((value) => {
      if (!value.isButton()) {
        throw Error('Keyboard mapping must only use scan codes, no hat or axis.')
      }
    })

    return mapping
  }

  /**
   * Create a mapping for a gamepad input device.
   *
   * @param entries {array} List of Key/Analog and hardware id pairs
   * @param options object containing a required name and uuid
   * @returns {Mapping}
   */
  static forGamepad (entries, options) {
    return new Mapping(options?.uuid, options?.name, entries)
  }

  /**
   * Deserialize a Mapping object from a CSV string.
   *
   * The CSV string is in SDL game controller mapping format.
   *
   * @param csv {string} the CSV string to deserialize
   * @returns {Mapping}
   */
  static fromCsv (csv) {
    const parts = csv?.split(',') || []
    const uuid = parts[0]
    const name = parts[1]
    const entries = []

    if (parts.length < 3) {
      throw Error(`invalid csv string: ${csv}`)
    }

    for (let i = 2; i < parts.length; i++) {
      // ignore an empty entry
      if (!parts[i]) {
        continue
      }

      const pair = parts[i].split(':')

      // platform: OS is always the last entry
      if (pair[0] === 'platform') {
        return new Mapping(uuid, name, entries)
      }

      if (pair.length !== 2) {
        throw Error(`expected a key:value entry at ${parts[i]}`)
      }

      entries.push([kGameControllerIdToKey.get(pair[0]), pair[1]])
    }

    throw Error('csv string must terminate with platform:PLATFORM_NAME')
  }
}

export const checkInstanceOfMapping = (instance) => {
  if (!instance || !(instance instanceof Mapping)) {
    throw Error('Expected an instance of Mapping')
  }
}

const kEmptyMappingValue = new MappingValue()
const kButtonRegEx = /^b(\d+)$/
const kHatRegEx = /^h(\d+)\.([1248])$/
const kAxisRegEx = /^(\+|-)?a(\d+)(~)?$/
const kGameControllerIdToKey = new Map([
  ['a', Key.A],
  ['b', Key.B],
  ['x', Key.X],
  ['y', Key.Y],
  ['leftshoulder', Key.LEFT_SHOULDER],
  ['rightshoulder', Key.RIGHT_SHOULDER],
  ['start', Key.START],
  ['back', Key.BACK],
  ['guide', Key.GUIDE],
  ['dpup', Key.DPAD_UP],
  ['dpright', Key.DPAD_RIGHT],
  ['dpdown', Key.DPAD_DOWN],
  ['dpleft', Key.DPAD_LEFT],
  ['rightstick', Key.RIGHT_STICK],
  ['leftstick', Key.LEFT_STICK],
  ['leftx', AnalogKey.LEFT_STICK_X],
  ['lefty', AnalogKey.LEFT_STICK_Y],
  ['rightx', AnalogKey.RIGHT_STICK_X],
  ['righty', AnalogKey.RIGHT_STICK_Y],
  ['lefttrigger', AnalogKey.LEFT_TRIGGER],
  ['righttrigger', AnalogKey.RIGHT_TRIGGER]
])

const kKeyToGameControllerId = invertMap(kGameControllerIdToKey)

// export for testing
export const kToGameControllerPlatform = new Map([
  ['darwin', 'Mac OS X'],
  ['win32', 'Windows'],
  ['linux', 'Linux']
])

export const createEmptyGameControllerCsv = (uuid) => `${uuid},*,platform:${kToGameControllerPlatform.get(process.platform)}`

const parseHardwareIdString = (hardwareId) => {
  const throwOnInvalidHardwareId = () => { throw Error('invalid hardware id: ' + hardwareId) }
  let result

  if (hardwareId.startsWith('b')) {
    result = kButtonRegEx.exec(hardwareId) || throwOnInvalidHardwareId()

    return MappingValue.forButton(parseInt(result[1]))
  } else if (hardwareId.startsWith('h')) {
    result = kHatRegEx.exec(hardwareId) || throwOnInvalidHardwareId()

    return MappingValue.forHat(parseInt(result[1]), parseInt(result[2]))
  }

  result = kAxisRegEx.exec(hardwareId) || throwOnInvalidHardwareId()

  return MappingValue.forAxis(parseInt(result[2]), result[3] ?? result[1])
}

export { Mapping }
