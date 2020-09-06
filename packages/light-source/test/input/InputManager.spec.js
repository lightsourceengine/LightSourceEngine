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
import { Keyboard } from '../../src/input/Keyboard.js'
import { rejects } from '../test-env.js'

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
    'axismotion',
    'buttondown',
    'buttonup',
    'hatdown',
    'hatup',
    'connected',
    'disconnected',
    'keydown',
    'keyup'
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
    it('should return null for unregistered uuid', () => {
      assert.isNull(inputManager.getMapping('unknown'))
    })
  })
  describe('setMapping()', () => {
    it('should add a new key mapping by uuid', () => {
      const mapping = new Mapping([])

      inputManager.setMapping('testUUID', mapping)

      assert.strictEqual(inputManager.getMapping('testUUID'), mapping)
    })
    it('should clear mapping given null', () => {
      inputManager.setMapping('testUUID', new Mapping([]))
      assert.isOk(inputManager.getMapping('testUUID'))
      inputManager.setMapping('testUUID', null)
      assert.isNull(inputManager.getMapping('testUUID'))
    })
    it('should throw Error for invalid uuid', () => {
      for (const input of [undefined, '', 0, {}]) {
        assert.throws(() => inputManager.setMapping(input, new Mapping([])))
      }
    })
    it('should throw Error for invalid key mapping', () => {
      for (const input of [undefined, '', 0, {}]) {
        assert.throws(() => inputManager.setMapping('testUUID', input))
      }
    })
  })
  describe('removeMapping()', () => {
    it('should remove key mapping by uuid', () => {
      inputManager.setMapping('testUUID', new Mapping([]))
      inputManager.removeMapping('testUUID')
      assert.isNull(inputManager.getMapping('testUUID'))
    })
    it('should be a noop for unregistered uuid', () => {
      inputManager.removeMapping('testUUID')
    })
  })
  describe('resolveMapping()', () => {
    it('should...', () => {
    })
  })
  describe('registerMapping()', () => {
    it('should...', () => {
    })
  })
  describe('axismotion callback', () => {
    it('should bubble unmapped event', () => {
      stage.plugin.callbacks.get('axismotion')({}, 0, 1)
    })
  })
  describe('buttondown callback', () => {
    it('should bubble unwrapped event', () => {
      stage.plugin.callbacks.get('buttondown')({}, 0)
    })
  })
  describe('buttonup callback', () => {
    it('should bubble unwrapped event', () => {
      stage.plugin.callbacks.get('buttonup')({}, 0)
    })
  })
  describe('connected callback', () => {
    it('should emit connected event', () => {
      const callback = sinon.stub()

      inputManager.once('connected', callback)
      stage.plugin.callbacks.get('connected')({})
      assert.isTrue(callback.called)
    })
  })
  describe('disconnected callback', () => {
    it('should emit disconnected event', () => {
      const callback = sinon.stub()

      inputManager.once('disconnected', callback)
      stage.plugin.callbacks.get('disconnected')({})
      assert.isTrue(callback.called)
    })
  })
  describe('keydown callback', () => {
    it('should bubble unmapped event', () => {
      // disable keyboard input mapping
      inputManager.setMapping(inputManager.keyboard.uuid, null)

      stage.plugin.callbacks.get('keydown')(new Keyboard(), 1, false)
    })
  })
  describe('keyup callback', () => {
    it('should bubble unwrapped event', () => {
      // disable keyboard input mapping
      inputManager.setMapping(inputManager.keyboard.uuid, null)

      stage.plugin.callbacks.get('keyup')(new Keyboard(), 1)
    })
  })
  describe('loadGameControllerDb()', () => {
    it('should load gamecontrollerdb.txt', async () => {
      await inputManager.loadGameControllerDb('test/resources/gamecontrollerdb.txt')
    })
    it('should throw if file not found', async () => {
      await rejects(inputManager.loadGameControllerDb('unknown'))
    })
  })
})
