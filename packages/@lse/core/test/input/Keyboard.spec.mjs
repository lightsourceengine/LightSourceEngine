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

import chai from 'chai'
import { Keyboard } from '../../src/input/Keyboard.mjs'
import { ScanCode } from '../../src/input/ScanCode.mjs'
import { Key } from '../../src/input/Key.mjs'
import { AnalogKey } from '../../src/input/AnalogKey.mjs'
import { kDefaultKeyboardName, kKeyboardUUID } from '../../src/input/InputCommon.mjs'
import { Mapping } from '../../src/input/Mapping.mjs'

const { assert } = chai
const uuid = '03000000c82d00000090000011010000'

describe('Keyboard', () => {
  let keyboard
  const state = new Map()
  beforeEach(() => {
    state.clear()
    keyboard = new Keyboard()
    keyboard.$setApi({
      getScanCodeState (scanCode) {
        return state.get(scanCode) ?? false
      }
    })
  })
  afterEach(() => {
    keyboard = null
  })
  describe('property: uuid', () => {
    it('should return keyboard uuid', () => {
      assert.equal(keyboard.uuid, kKeyboardUUID)
    })
  })
  describe('property: name', () => {
    it('should return default keyboard name', () => {
      assert.equal(keyboard.name, kDefaultKeyboardName)
    })
  })
  describe('property: id', () => {
    it('should return instance id of 0', () => {
      assert.equal(keyboard.id, 0)
    })
  })
  describe('property: type', () => {
    it('should return keyboard type', () => {
      assert.equal(keyboard.type, 'keyboard')
    })
  })
  describe('getScanCodeState()', () => {
    it('should return true if key is down', () => {
      state.set(ScanCode.RSHIFT, true)
      assert.isTrue(keyboard.getScanCodeState(ScanCode.RSHIFT))
    })
    it('should return false if key is not down', () => {
      assert.isFalse(keyboard.getScanCodeState(ScanCode.RSHIFT))
    })
    it('should return false for invalid scanCode', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.isFalse(keyboard.getScanCodeState(input))
      }
    })
  })
  describe('isKeyDown()', () => {
    it('should return true if key is down', () => {
      state.set(ScanCode.Z, true)
      assert.isTrue(keyboard.isKeyDown(Key.A))
    })
    it('should return false if key is not down', () => {
      assert.isFalse(keyboard.isKeyDown(Key.A))
    })
    it('should return false for invalid key', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.isFalse(keyboard.isKeyDown(input))
      }
    })
  })
  describe('getAnalogValue()', () => {
    const analogMapping = Mapping.forKeyboard([
      [AnalogKey.LEFT_STICK_X, ScanCode.A],
      [AnalogKey.LEFT_STICK_Y, ScanCode.B],
      [AnalogKey.RIGHT_STICK_X, ScanCode.C],
      [AnalogKey.RIGHT_STICK_Y, ScanCode.D],
      [AnalogKey.LEFT_TRIGGER, ScanCode.E],
      [AnalogKey.RIGHT_TRIGGER, ScanCode.F]
    ])
    it('should return 1 or -1 when underlying hardware key is pressed', () => {
      keyboard.$setMapping(analogMapping)

      state.set(ScanCode.A, true)
      state.set(ScanCode.B, true)
      state.set(ScanCode.C, true)
      state.set(ScanCode.D, true)
      state.set(ScanCode.E, true)
      state.set(ScanCode.F, true)

      assert.equal(keyboard.getAnalogValue(AnalogKey.LEFT_STICK_X), -1)
      assert.equal(keyboard.getAnalogValue(AnalogKey.LEFT_STICK_Y), 1)
      assert.equal(keyboard.getAnalogValue(AnalogKey.RIGHT_STICK_X), -1)
      assert.equal(keyboard.getAnalogValue(AnalogKey.RIGHT_STICK_Y), 1)
      assert.equal(keyboard.getAnalogValue(AnalogKey.LEFT_TRIGGER), 1)
      assert.equal(keyboard.getAnalogValue(AnalogKey.RIGHT_TRIGGER), 1)
    })
    it('should return 0 when underlying scanCode is not down', () => {
      keyboard.$setMapping(analogMapping)
      for (const analogKey of Object.values(AnalogKey)) {
        assert.equal(keyboard.getAnalogValue(analogKey), 0)
      }
    })
    it('should return 0 for invalid key', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.equal(keyboard.getAnalogValue(input), 0)
      }
    })
  })
  describe('$setMapping', () => {
    it('should set mapping', () => {
      const name = 'Test Keyboard'

      assert.equal(keyboard.name, kDefaultKeyboardName)
      keyboard.$setMapping(Mapping.forKeyboard([], { name }))
      assert.equal(keyboard.name, name)
    })
    it('should reset to default mapping for null', () => {
      const name = 'Test Keyboard'

      keyboard.$setMapping(Mapping.forKeyboard([], { name }))
      assert.equal(keyboard.name, name)
      keyboard.$setMapping(null)
      assert.equal(keyboard.name, kDefaultKeyboardName)
    })
    it('should throw Error for invalid mapping instance', () => {
      assert.throws(() => keyboard.$setMapping({}))
    })
    it('should throw Error for invalid uuid', () => {
      assert.throws(() => keyboard.$setMapping(Mapping.forGamepad([], { name: '*', uuid })))
    })
  })
})
