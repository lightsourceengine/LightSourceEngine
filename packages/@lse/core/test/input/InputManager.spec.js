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
import { EventNames } from '../../src/event/index.js'
import { ScanCode } from '../../src/input/ScanCode.js'
import { kDefaultKeyboardName, kKeyboardUUID } from '../../src/input/InputCommon.js'
import { Direction } from '../../src/input/Direction.js'
import { Key } from '../../src/input/Key.js'

const { assert } = chai

const createMockStage = () => {
  const scene = {
    $emit: sinon.stub()
  }

  const plugin = {
    keyboard: {},
    gamepads: [],
    callbacks: new Map(),
    getKeyboard () {
      return this.keyboard
    },
    getGamepads () {
      return this.gamepads
    },
    setCallback (type, callback) {
      this.callbacks.set(type, callback)
    },
    resetCallbacks () {
      this.callbacks.clear()
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
    }
  }

  return {
    plugin,
    scene,
    $emit: sinon.stub(),
    getScene () {
      return scene
    }
  }
}
const assertCallbacksRegistered = (plugin) => {
  assert.containsAllKeys(plugin.callbacks, [
    'keyboard:button',
    'gamepad:status',
    'gamepad:axis',
    'gamepad:hat',
    'gamepad:button',
    'gamepad:button-mapped',
    'gamepad:axis-mapped'
  ])
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
      assert.lengthOf(stage.plugin.callbacks, 0)
    })
  })
  describe('keyboard', () => {
    it('should return input manager keyboard instance', () => {
      assert.notStrictEqual(inputManager.keyboard, stage.plugin.keyboard)
    })
  })
  describe('gamepads', () => {
    it('should return gamepads retrieved from adapter', () => {
      assert.lengthOf(inputManager.gamepads, 0)
      stage.plugin.gamepads = [{}]
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
  describe('rawaxismotion event', () => {
    it('should emit rawaxismotion event on gamepad:axis plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.rawaxismotion, 'gamepad:axis', [{}, 0, 1])
    })
  })
  describe('rawbuttondown event', () => {
    it('should emit rawbuttondown event on gamepad:button plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.rawbuttondown, 'gamepad:button', [{}, 0, 1])
    })
  })
  describe('rawbuttonup event', () => {
    it('should emit rawbuttonup event on gamepad:button plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.rawbuttonup, 'gamepad:button', [{}, 0, 0])
    })
  })
  describe('rawhatmotion event', () => {
    it('should emit rawhatmotion event on gamepad:hat plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.rawhatmotion, 'gamepad:hat', [{}, 0, 1])
    })
  })
  describe('rawkeyup event', () => {
    it('should emit rawkeyup event on keyboard:button plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.rawkeyup, 'keyboard:button', [{}, 0, 0, 0])
    })
  })
  describe('rawkeydown callback', () => {
    it('should emit rawkeydown event on keyboard:button plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.rawkeydown, 'keyboard:button', [{}, 0, 1, 0])
    })
  })
  describe('connected event', () => {
    it('should emit connected event on gamepad:status plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.connected, 'gamepad:status', [{}, true])
    })
  })
  describe('disconnected event', () => {
    it('should emit disconnected event on gamepad:status plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.disconnected, 'gamepad:status', [{}, false])
    })
  })
  describe('keyup event', () => {
    it('should emit keyup event on gamepad:button-mapped plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.keyup, 'gamepad:button-mapped', [{}, 0, 0])
    })
    it('should emit keyup event on keyboard:button plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.keyup, 'keyboard:button', [{}, ScanCode.Z, 0, 0])
    })
  })
  describe('keydown event', () => {
    it('should emit keydown event on gamepad:button-mapped plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.keydown, 'gamepad:button-mapped', [{}, 0, 1, 0])
    })
    it('should emit keydown event on keyboard:button plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.keydown, 'keyboard:button', [{}, ScanCode.Z, 1, 0])
    })
  })
  describe('analogmotion event', () => {
    it('should emit analogmotion event on gamepad:hat plugin event', () => {
      testEventTriggeredByPluginCallback(EventNames.analogmotion, 'gamepad:axis-mapped', [{}, 0, 1])
    })
  })

  const uuid = ''.padStart(32, '1')

  const testEventTriggeredByPluginCallback = (event, callbackId, callbackArgs) => {
    const callback = sinon.stub()

    inputManager.once(event, callback)
    stage.plugin.callbacks.get(callbackId)(...callbackArgs)
    assert.isTrue(callback.called)
  }
})
