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
import { existsSync } from 'fs'
import sinon from 'sinon'
import { InputManager } from '../../src/input/InputManager.mjs'
import { Mapping } from '../../src/input/Mapping.mjs'
import { ScanCode } from '../../src/input/ScanCode.mjs'
import { kDefaultKeyboardName, kKeyboardUUID } from '../../src/input/InputCommon.mjs'
import { Direction } from '../../src/input/Direction.mjs'
import { Key } from '../../src/input/Key.mjs'
import { EventName } from '../../src/event/EventName.mjs'

const { assert } = chai

const uuid = ''.padStart(32, '1')

const callbackProperties = [
  'onKeyboardScanCode',
  'onGamepadStatus',
  'onGamepadAxis',
  'onGamepadHat',
  'onGamepadButton',
  'onGamepadButtonMapped',
  'onGamepadAxisMapped'
]

const createMockStage = () => {
  const plugin = {
    keyboard: {},
    gamepads: [],
    getGamepadInstanceIds () {
      return []
    },
    getGamepadInfo (id) {
      return { id }
    },
    resetCallbacks () {
      callbackProperties.forEach(callbackProperty => { this[callbackProperty] = null })
    },
    loadGameControllerMappings (file) {
      try {
        // Simulate the SDL api that would read the file.
        return existsSync(file)
      } catch (e) {
        return false
      }
    },
    getGameControllerMapping (uuid) {
      return null
    },
    testAddGamepad (instanceId) {
      this.onGamepadStatus(instanceId, true)
    }
  }

  return {
    plugin,
    scene: {
      $emit: sinon.stub()
    },
    $emit: sinon.stub(),
    get $scene () {
      return this.scene
    }
  }
}

const assertCallbacksRegistered = (plugin) => {
  callbackProperties.forEach(prop => assert.isFunction(plugin[prop]))
}

const assertCallbacksUnregistered = (plugin) => {
  callbackProperties.forEach(prop => assert.isNull(plugin[prop]))
}

describe('InputManager', () => {
  let inputManager
  let stage
  beforeEach(() => {
    stage = createMockStage()
    inputManager = new InputManager(stage)
    inputManager.$setPlugin(stage.plugin)
    inputManager.$attach()
  })
  afterEach(() => {
    inputManager.$detach()
    inputManager = null
    stage = null
  })
  describe('$attach()', () => {
    it('should attach to platform plugin', () => {
      assert.isTrue(inputManager.isEnabled())
      assert.isOk(inputManager.keyboard)
      assertCallbacksRegistered(stage.plugin)
    })
  })
  describe('$detach()', () => {
    it('should detach from the platform plugin', () => {
      inputManager.$detach()
      assert.isTrue(inputManager.isEnabled())
      assert.isOk(inputManager.keyboard)
      assertCallbacksUnregistered(stage.plugin)
    })
  })
  describe('keyboard', () => {
    it('should return input manager keyboard instance', () => {
      assert.notStrictEqual(inputManager.keyboard, stage.plugin.keyboard)
    })
  })
  describe('gamepads', () => {
    it('should return connected gamepads', () => {
      assert.lengthOf(inputManager.gamepads, 0)
      stage.plugin.testAddGamepad(1)
      assert.lengthOf(inputManager.gamepads, 1)
    })
  })
  describe('getMapping()', () => {
    it('should return keyboard mapping for keyboard uuid', () => {
      assert.instanceOf(inputManager.getMapping(kKeyboardUUID), Mapping)
    })
    it('should return null for unregistered uuid', () => {
      assert.isNull(inputManager.getMapping(uuid))
    })
  })
  describe('setMapping()', () => {
    it('should set keyboard mapping', () => {
      inputManager.setMapping(Mapping.forKeyboard([], { name: 'test' }))

      assert.equal(inputManager.getMapping(kKeyboardUUID).name, 'test')
      assert.equal(inputManager.keyboard.name, 'test')
    })
    it('should set gamepad mapping', () => {
      assert.isFalse(inputManager.hasMapping(uuid))
      inputManager.setMapping(Mapping.forGamepad([], { name: 'test', uuid }))
      assert.isTrue(inputManager.hasMapping(uuid))
    })
    it('should throw Error for invalid mapping argument', () => {
      for (const input of [undefined, null, '', 'blah', 0, {}, NaN, []]) {
        assert.throws(() => inputManager.setMapping(input))
      }
    })
  })
  describe('removeMapping()', () => {
    it('should remove keyboard mapping by uuid and restore default keyboard mapping', () => {
      inputManager.setMapping(Mapping.forKeyboard([], { name: 'test' }))
      inputManager.removeMapping(kKeyboardUUID)
      assert.equal(inputManager.getMapping(kKeyboardUUID).name, kDefaultKeyboardName)
    })
    it('should remove mapping', () => {
      inputManager.setMapping(Mapping.forGamepad([], { name: 'test', uuid }))
      assert.isTrue(inputManager.hasMapping(uuid))
      inputManager.removeMapping(uuid)
      assert.isFalse(inputManager.hasMapping(uuid))
    })
    it('should be a noop for unregistered uuid', () => {
      assert.isFalse(inputManager.hasMapping(uuid))
      inputManager.removeMapping(uuid)
    })
  })
  describe('hasMapping()', () => {
    it('should always return true for keyboard uuid', () => {
      assert.isTrue(inputManager.hasMapping(kKeyboardUUID))
    })
    it('should return true for registered uuid', () => {
      inputManager.setMapping(Mapping.forGamepad([], { name: 'test', uuid }))
      assert.isTrue(inputManager.hasMapping(uuid))
    })
    it('should return false for device with no mapping', () => {
      assert.isFalse(inputManager.hasMapping(uuid))
    })
    it('should return false for invalid uuids', () => {
      for (const input of [undefined, null, '', 'blah', 0, {}, NaN, []]) {
        assert.isFalse(inputManager.hasMapping(input))
      }
    })
  })
  describe('setNavigationMapping()', () => {
    it('should set navigation mapping', () => {
      inputManager.setNavigationMapping([
        [Key.DPAD_DOWN, Direction.DOWN]
      ])
    })
    it('should throw error for invalid mapping', () => {
      for (const input of ['', null, undefined, NaN, 1, {}]) {
        assert.throws(() => inputManager.setNavigationMapping(input))
      }
    })
  })
  describe('resetNavigationMapping()', () => {
    it('should reset navigation mapping', () => {
      inputManager.setNavigationMapping([
        [Key.DPAD_DOWN, Direction.DOWN]
      ])
      inputManager.resetNavigationMapping()
    })
    it('should do nothing if no navigation mapping set by user', () => {
      inputManager.resetNavigationMapping()
    })
  })
  describe('axis-motion event', () => {
    it('should emit axis-motion event for onGamepadAxis plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.axisMotion, 'onGamepadAxis', [0, 0, 1])
    })
  })
  describe('button-down event', () => {
    it('should emit button-down event for onGamepadButton plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.buttonDown, 'onGamepadButton', [0, 0, 1])
    })
  })
  describe('button-up event', () => {
    it('should emit button-up event for onGamepadButton plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.buttonUp, 'onGamepadButton', [0, 0, 0])
    })
  })
  describe('hat-motion event', () => {
    it('should emit hat-motion event for onGamepadHat plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.hatMotion, 'onGamepadHat', [0, 0, 1])
    })
  })
  describe('scan-code-up event', () => {
    it('should emit scan-code-up event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.scanCodeUp, 'onKeyboardScanCode', [0, 0, 0])
    })
  })
  describe('scan-code-down callback', () => {
    it('should emit scan-code-up event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.scanCodeDown, 'onKeyboardScanCode', [0, 1, 0])
    })
  })
  describe('connected event', () => {
    it('should emit connected event for onGamepadStatus plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.connected, 'onGamepadStatus', [0, true])
    })
  })
  describe('disconnected event', () => {
    it('should emit disconnected event for onGamepadStatus plugin callback', () => {
      stage.plugin.testAddGamepad(1)
      testEventTriggeredByPluginCallback(EventName.disconnected, 'onGamepadStatus', [1, false])
    })
  })
  describe('key-up event', () => {
    it('should emit key-up event for onGamepadButtonMapped plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.keyUp, 'onGamepadButtonMapped', [0, 0, 0])
    })
    it('should emit key-up event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.keyUp, 'onKeyboardScanCode', [ScanCode.Z, 0, 0])
    })
  })
  describe('key-down event', () => {
    it('should emit key-down event for onGamepadButtonMapped plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.keyDown, 'onGamepadButtonMapped', [0, 0, 1, 0])
    })
    it('should emit key-down event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.keyDown, 'onKeyboardScanCode', [ScanCode.Z, 1, 0])
    })
  })
  describe('analog-motion event', () => {
    it('should emit analog-motion event for onGamepadAxisMapped plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.analogMotion, 'onGamepadAxisMapped', [0, 0, 1])
    })
  })

  const testEventTriggeredByPluginCallback = (event, callbackId, callbackArgs) => {
    const callback = sinon.stub()

    inputManager.once(event, callback)
    stage.plugin[callbackId](...callbackArgs)
    assert.isTrue(callback.called)
  }
})
