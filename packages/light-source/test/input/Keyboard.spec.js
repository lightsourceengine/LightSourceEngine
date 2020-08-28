/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { Keyboard } from '../../src/input/Keyboard'
import { parseSystemMapping } from '../../src/input/parseSystemMapping'
import { ScanCode } from '../../src/input/ScanCode'
import { Key } from '../../src/input/Key'
import { MappingType } from '../../src/input/MappingType'

describe('Keyboard', () => {
  let keyboard = null
  beforeEach(() => {
    keyboard = new Keyboard()
  })
  afterEach(() => {
    keyboard = null
  })
  describe('mapping', () => {
    it('should return a valid native mapping string', () => {
      const [mapping, uuid] = parseSystemMapping(keyboard.$getGameControllerMapping())

      assert.equal(uuid, keyboard.uuid)
      assert.equal(mapping.name, MappingType.Standard)

      assert.equal(mapping.getKeyForButton(ScanCode.RETURN), Key.START)
      assert.equal(mapping.getKeyForButton(ScanCode.RSHIFT), Key.SELECT)
      assert.equal(mapping.getKeyForButton(ScanCode.Q), Key.L1)
      assert.equal(mapping.getKeyForButton(ScanCode.W), Key.R1)
      assert.equal(mapping.getKeyForButton(ScanCode.Z), Key.B)
      assert.equal(mapping.getKeyForButton(ScanCode.X), Key.A)
      assert.equal(mapping.getKeyForButton(ScanCode.S), Key.X)
      assert.equal(mapping.getKeyForButton(ScanCode.A), Key.Y)
      assert.equal(mapping.getKeyForButton(ScanCode.UP), Key.UP)
      assert.equal(mapping.getKeyForButton(ScanCode.DOWN), Key.DOWN)
      assert.equal(mapping.getKeyForButton(ScanCode.LEFT), Key.LEFT)
      assert.equal(mapping.getKeyForButton(ScanCode.RIGHT), Key.RIGHT)
    })
  })
  describe('isButtonDown()', () => {
    it('should always return false when native keyboard is not set', () => {
      assert.isFalse(keyboard.isButtonDown(ScanCode.X))
    })
    it('should consult native keyboard for key down status', () => {
      // set a mock native keyboard that has the X button down
      keyboard.$setNative({
        isButtonDown (scanCode) {
          return scanCode === ScanCode.X
        }
      })

      assert.isFalse(keyboard.isButtonDown(ScanCode.Y))
      assert.isTrue(keyboard.isButtonDown(ScanCode.X))
    })
  })
})
