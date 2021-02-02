/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { existsSync } from 'fs'
import sinon from 'sinon'
import { InputManager } from '../../src/input/InputManager.js'
import { Mapping } from '../../src/input/Mapping.js'
import { ScanCode } from '../../src/input/ScanCode.js'
import { kDefaultKeyboardName, kKeyboardUUID } from '../../src/input/InputCommon.js'
import { Direction } from '../../src/input/Direction.js'
import { Key } from '../../src/input/Key.js'
import { EventName } from '../../src/event/EventName.js'

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
    getScene (displayId) {
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
  describe('on-axis-motion event', () => {
    it('should emit on-axis-motion event for onGamepadAxis plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onAxisMotion, 'onGamepadAxis', [0, 0, 1])
    })
  })
  describe('on-button-down event', () => {
    it('should emit on-button-down event for onGamepadButton plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onButtonDown, 'onGamepadButton', [0, 0, 1])
    })
  })
  describe('on-button-up event', () => {
    it('should emit on-button-up event for onGamepadButton plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onButtonUp, 'onGamepadButton', [0, 0, 0])
    })
  })
  describe('on-hat-motion event', () => {
    it('should emit on-hat-motion event for onGamepadHat plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onHatMotion, 'onGamepadHat', [0, 0, 1])
    })
  })
  describe('on-scan-code-up event', () => {
    it('should emit on-scan-code-up event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onScanCodeUp, 'onKeyboardScanCode', [0, 0, 0])
    })
  })
  describe('on-scan-code-down callback', () => {
    it('should emit on-scan-code-up event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onScanCodeDown, 'onKeyboardScanCode', [0, 1, 0])
    })
  })
  describe('on-connected event', () => {
    it('should emit on-connected event for onGamepadStatus plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onConnected, 'onGamepadStatus', [0, true])
    })
  })
  describe('on-disconnected event', () => {
    it('should emit on-disconnected event for onGamepadStatus plugin callback', () => {
      stage.plugin.testAddGamepad(1)
      testEventTriggeredByPluginCallback(EventName.onDisconnected, 'onGamepadStatus', [1, false])
    })
  })
  describe('on-key-up event', () => {
    it('should emit on-key-up event for onGamepadButtonMapped plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onKeyUp, 'onGamepadButtonMapped', [0, 0, 0])
    })
    it('should emit on-key-up event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onKeyUp, 'onKeyboardScanCode', [ScanCode.Z, 0, 0])
    })
  })
  describe('on-key-down event', () => {
    it('should emit on-key-down event for onGamepadButtonMapped plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onKeyDown, 'onGamepadButtonMapped', [0, 0, 1, 0])
    })
    it('should emit on-key-down event for onKeyboardScanCode plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onKeyDown, 'onKeyboardScanCode', [ScanCode.Z, 1, 0])
    })
  })
  describe('on-analog-motion event', () => {
    it('should emit on-analog-motion event for onGamepadAxisMapped plugin callback', () => {
      testEventTriggeredByPluginCallback(EventName.onAnalogMotion, 'onGamepadAxisMapped', [0, 0, 1])
    })
  })

  const testEventTriggeredByPluginCallback = (event, callbackId, callbackArgs) => {
    const callback = sinon.stub()

    inputManager.once(event, callback)
    stage.plugin[callbackId](...callbackArgs)
    assert.isTrue(callback.called)
  }
})
