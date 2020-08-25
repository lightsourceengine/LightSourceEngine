/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { isNumber } from '../util'
import { MappingType } from './MappingType'

const buttonRegEx = /^b(\d+)$/
const hatRegEx = /^h(\d+)\.([1248])$/
const axisRegEx = /^(\+|-)?a(\d+)(~)?$/

const $buttonMap = Symbol('buttonMap')
const $hatMap = Symbol('hatMap')

export class Mapping {
  /**
   *
   * @param args
   */
  constructor (...args) {
    let name
    let entries

    if (args.length === 1) {
      name = MappingType.Standard
      entries = args[0]
    } else if (args.length === 2) {
      name = args[0]
      entries = args[1]
    }

    if (!name || typeof name !== 'string') {
      throw Error('Mapping: name must be a string')
    }

    if (!entries || !Array.isArray(entries)) {
      throw Error('Mapping: entries must be an array')
    }

    this.name = name
    this[$buttonMap] = new Map()
    this[$hatMap] = new Map()

    let result

    for (const [hardwareKey, mappedKey] of entries) {
      if (!isNumber(mappedKey)) {
        throw Error(`Mapping: Mapped key must be a number (${mappedKey})`)
      }

      if (typeof hardwareKey === 'string') {
        result = buttonRegEx.exec(hardwareKey)

        if (result) {
          this[$buttonMap].set(parseInt(result[1]), mappedKey)
          continue
        }

        result = axisRegEx.exec(hardwareKey)

        if (result) {
          // TODO: add axis
          continue
        }

        result = hatRegEx.exec(hardwareKey)

        if (result) {
          const hatIndex = parseInt(result[1])

          if (!this[$hatMap].has(hatIndex)) {
            this[$hatMap].set(hatIndex, new Map())
          }

          this[$hatMap].get(hatIndex).set(parseInt(result[2]), mappedKey)

          continue
        }
      } else if (isNumber(hardwareKey)) {
        this[$buttonMap].set(hardwareKey, mappedKey)
        continue
      }

      throw Error(`Mapping: Unknown hardware key string: ${hardwareKey}`)
    }
  }

  /**
   *
   * @param button
   * @returns {number}
   */
  getKeyForButton (button) {
    return this[$buttonMap].has(button) ? this[$buttonMap].get(button) : -1
  }

  /**
   *
   * @param axis
   * @param value
   * @returns {number}
   */
  getKeyForAxis (axis, value) {
    return -1
  }

  /**
   *
   * @param hat
   * @param value
   * @returns {*}
   */
  getKeyForHat (hat, value) {
    // note: hat values are never 0, so || -1 will work
    return this[$hatMap].has(hat) ? this[$hatMap].get(hat).get(value) || -1 : -1
  }
}
