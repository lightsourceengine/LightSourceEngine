/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { Gamepad } from '../../src/input/Gamepad.js'
import { Key } from '../../src/input/Key.js'
import { AnalogKey } from '../../src/input/AnalogKey.js'

const { assert } = chai
const gamepadId = 0
const gamepadInfo = {
  id: gamepadId,
  type: 'gamepad',
  name: 'Test Gamepad',
  uuid: '03000000c82d00000090000011010000',
  buttonCount: 2,
  hatCount: 3,
  axisCount: 4
}

describe('Gamepad', () => {
  let gamepad
  let mockApi
  beforeEach(() => {
    mockApi = {
      getGamepadInfo (id) {
        return gamepadInfo
      },
      isKeyDown (id, key) {
        return key === Key.START
      },
      getAnalogValue (id, analogKey) {
        return analogKey === AnalogKey.LEFT_STICK_X ? 1 : 0
      },
      getButtonState (id, button) {
        return button === 0
      },
      getAxisState (id, axis) {
        return axis === 0 ? 1 : 0
      },
      getHatState (id, hat) {
        return hat === 0 ? 1 : 0
      }
    }
    gamepad = new Gamepad(0, mockApi)
  })
  afterEach(() => {
    gamepad = null
  })
  describe('property: uuid', () => {
    it('should return gamepad uuid', () => {
      assert.equal(gamepad.uuid, gamepadInfo.uuid)
    })
  })
  describe('property: name', () => {
    it('should return default gamepad name', () => {
      assert.equal(gamepad.name, gamepadInfo.name)
    })
  })
  describe('property: id', () => {
    it('should return gamepad instance id of 0', () => {
      assert.equal(gamepad.id, gamepadId)
    })
  })
  describe('property: type', () => {
    it('should return gamepad type', () => {
      assert.equal(gamepad.type, 'gamepad')
    })
  })
  describe('property: buttonCount', () => {
    it('should return gamepad buttonCount', () => {
      assert.equal(gamepad.buttonCount, gamepadInfo.buttonCount)
    })
  })
  describe('property: axisCount', () => {
    it('should return gamepad axisCount', () => {
      assert.equal(gamepad.axisCount, gamepadInfo.axisCount)
    })
  })
  describe('property: hatCount', () => {
    it('should return gamepad hatCount', () => {
      assert.equal(gamepad.hatCount, gamepadInfo.hatCount)
    })
  })
  describe('isKeyDown()', () => {
    it('should return true if key is down', () => {
      assert.isTrue(gamepad.isKeyDown(Key.START))
    })
    it('should return false if key is not down', () => {
      assert.isFalse(gamepad.isKeyDown(Key.GUIDE))
    })
    it('should return false for invalid key', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.isFalse(gamepad.isKeyDown(input))
      }
    })
  })
  describe('getAnalogValue()', () => {
    it('should return 1 for left stick x', () => {
      assert.equal(gamepad.getAnalogValue(AnalogKey.LEFT_STICK_X), 1)
    })
    it('should return 0 for left stick y', () => {
      assert.equal(gamepad.getAnalogValue(AnalogKey.LEFT_STICK_Y), 0)
    })
    it('should return 0 for invalid key', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.equal(gamepad.getAnalogValue(input), 0)
      }
    })
  })
  describe('getButtonState()', () => {
    it('should return true if button is down', () => {
      assert.isTrue(gamepad.getButtonState(0))
    })
    it('should return false if button is not down', () => {
      assert.isFalse(gamepad.getButtonState(1))
    })
    it('should return false for invalid button', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.isFalse(gamepad.getButtonState(input))
      }
    })
  })
  describe('getAxisState()', () => {
    it('should return 1 for axis 0', () => {
      assert.equal(gamepad.getAxisState(0), 1)
    })
    it('should return 0 for axis 1', () => {
      assert.equal(gamepad.getAxisState(1), 0)
    })
    it('should return 0 for invalid axis value', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.equal(gamepad.getAxisState(input), 0)
      }
    })
  })
  describe('getHatState()', () => {
    it('should return 1 for hat 0', () => {
      assert.equal(gamepad.getHatState(0), 1)
    })
    it('should return 0 for hat 1', () => {
      assert.equal(gamepad.getHatState(1), 0)
    })
    it('should return 0 for invalid hat value', () => {
      for (const input of [-1000, {}, null, '', undefined]) {
        assert.equal(gamepad.getAxisState(input), 0)
      }
    })
  })
})
