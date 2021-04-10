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
import { kToGameControllerPlatform, Mapping } from '../../src/input/Mapping.js'
import { Key } from '../../src/input/Key.js'
import { ScanCode } from '../../src/input/ScanCode.js'
import { kDefaultKeyboardName, kKeyboardUUID } from '../../src/input/InputCommon.js'
import { AnalogKey } from '../../src/input/AnalogKey.js'

const { assert } = chai
const uuid = '5231b533ba17478798a3f2df37de2aD7'
const name = 'Test Device'

describe('Mapping', () => {
  let mapping
  const initMapping = (entries = []) => (mapping = Mapping.forGamepad(entries, { name, uuid }))

  describe('constructor', () => {
    it('should set test type, uuid and displayName', () => {
      initMapping()
      assert.equal(mapping.uuid, uuid)
      assert.equal(mapping.name, name)
    })
    it('should parse button integer', () => {
      testButtonParsing(2)
    })
    it('should parse button string', () => {
      testButtonParsing('b2')
    })
    it('should parse button object', () => {
      testButtonParsing({ button: 2 })
    })
    it('should parse button value', () => {
      testButtonParsing(Mapping.Value.forButton(2))
    })
    it('should parse hat string', () => {
      testHatParsing('h1.4')
    })
    it('should parse hat object', () => {
      testHatParsing({ hat: 1, meta: 4 })
    })
    it('should parse hat value', () => {
      testHatParsing(Mapping.Value.forHat(1, 4))
    })
    it('should parse axis string', () => {
      testAxisParsing('a3', '')
      testAxisParsing('+a3', '+')
      testAxisParsing('-a3', '-')
      testAxisParsing('a3~', '~')
    })
    it('should parse axis object', () => {
      testAxisParsing({ axis: 3, meta: '~' }, '~')
    })
    it('should parse axis value', () => {
      testAxisParsing(Mapping.Value.forAxis(3, '+'), '+')
    })
    it('should throw Error for no arguments', () => {
      assert.throws(() => new Mapping())
    })
    it('should throw Error for invalid uuid', () => {
      for (const invalidUUID of ['', null, {}, -1]) {
        assert.throws(() => new Mapping(invalidUUID, name, []))
      }
    })
    it('should throw Error for invalid name', () => {
      for (const invalidName of [null, '', undefined, -1, {}]) {
        assert.throws(() => new Mapping(uuid, invalidName, []))
      }
    })
    it('should throw Error invalid hardware id strings', () => {
      for (const input of ['', '2', 'h0.', 'b', 'a', 'h0.a', null, undefined, {}, NaN]) {
        assert.throws(() => initMapping([[Key.START, input]]))
      }
    })
  })
  describe('getValue()', () => {
    it('should return value mapped to Key A', () => {
      initMapping([[Key.A, ScanCode.RETURN]])

      const value = mapping.getValue(Key.A)

      assert.instanceOf(value, Mapping.Value)
      assert.isTrue(value.isButton())
      assert.equal(value.button, ScanCode.RETURN)
    })
    it('should return empty value for unmapped Key B', () => {
      initMapping([[Key.A, ScanCode.RETURN]])

      const value = mapping.getValue(Key.B)

      assert.instanceOf(value, Mapping.Value)
      assert.isFalse(value.isButton())
      assert.isFalse(value.isAxis())
      assert.isFalse(value.isHat())
    })
  })
  describe('getKeyForButton()', () => {
    it('should ', () => {
      initMapping([[Key.A, ScanCode.RETURN]])

      assert.equal(mapping.getKeyForButton(ScanCode.RETURN), Key.A)
    })
    it('should return empty value for unmapped Key B', () => {
      initMapping([[Key.A, ScanCode.RETURN]])

      assert.equal(mapping.getKeyForButton(ScanCode.LEFT), -1)
    })
  })
  describe('toCsv()', () => {
    it('should...', () => {
      initMapping([[Key.A, ScanCode.RETURN]])

      assert.equal(mapping.toCsv(),
        `${uuid},${name},a:b${ScanCode.RETURN},platform:${kToGameControllerPlatform.get(process.platform)},`)
    })
  })
  describe('fromCsv()', () => {
    it('should parse csv with no mapping entries', () => {
      const csv = `${uuid},${name},righttrigger:b3,lefttrigger:a1,platform:*,`
      const mapping = Mapping.fromCsv(csv)

      assert.equal(mapping.uuid, uuid)
      assert.equal(mapping.name, name)
      assert.equal(mapping.getValue(AnalogKey.RIGHT_TRIGGER).button, 3)
      assert.equal(mapping.getValue(AnalogKey.LEFT_TRIGGER).axis, 1)
    })
    it('should parse csv with no mapping entries', () => {
      const csv = `${uuid},${name},platform:*,`
      const mapping = Mapping.fromCsv(csv)

      assert.equal(mapping.uuid, uuid)
      assert.equal(mapping.name, name)
    })
    it('should throw Error for malformed csv strings', () => {
      const values = [
        ',',
        `${name}`,
        `${name},${uuid}`,
        `${name},${uuid},righttrigger:b3`,
        `${name},${uuid},righttrigger:,platform:*`,
        `${name},${uuid},:b3,platform:*`,
        `${name},${uuid},:,platform:*`,
        `${name},${uuid},righttrigger:b3:righttrigger:b3,platform:*`
      ]

      for (const input of values) {
        assert.throws(() => Mapping.fromCsv(input))
      }
    })
    it('should throw Error for invalid csv argument', () => {
      for (const input of ['', null, undefined, 3, {}, [], NaN]) {
        assert.throws(() => Mapping.fromCsv(input))
      }
    })
  })
  describe('forKeyboard()', () => {
    it('should create a keyboard mapping', () => {
      const mapping = Mapping.forKeyboard([[Key.A, ScanCode.RETURN]])

      assert.equal(mapping.uuid, kKeyboardUUID)
      assert.equal(mapping.name, kDefaultKeyboardName)
    })
    it('should create a keyboard mapping with custom name', () => {
      const mapping = Mapping.forKeyboard([[Key.A, ScanCode.RETURN]], { name })

      assert.equal(mapping.uuid, kKeyboardUUID)
      assert.equal(mapping.name, name)
    })
  })
  describe('forGamepad()', () => {
    it('should create a gamepad mapping', () => {
      const mapping = Mapping.forGamepad([[Key.A, ScanCode.RETURN]], { name, uuid })

      assert.equal(mapping.uuid, uuid)
      assert.equal(mapping.name, name)
    })
  })

  const testButtonParsing = (input) => {
    initMapping([[Key.START, input]])

    const value = mapping.getValue(Key.START)

    assert.isTrue(value.isButton())
    assert.equal(value.button, 2)
  }

  const testHatParsing = (input) => {
    initMapping([[Key.START, input]])

    const value = mapping.getValue(Key.START)

    assert.isTrue(value.isHat())
    assert.equal(value.hat, 1)
    assert.equal(value.meta, 4)
  }

  const testAxisParsing = (input, expectedMeta) => {
    initMapping([[Key.START, input]])

    const value = mapping.getValue(Key.START)

    assert.isTrue(value.isAxis())
    assert.equal(value.axis, 3)
    assert.equal(value.meta, expectedMeta)
  }
})
