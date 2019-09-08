/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { InputManager } from '../../src/input/InputManager'
import {
  $adapter,
  $attach,
  $detach,
  $emit,
  $init,
  $scene
} from '../../src/util/InternalSymbols'
import sinon from 'sinon'
import { Mapping } from '../../src/input/Mapping'
import { Keyboard } from '../../src/input/Keyboard'

const gameControllerDb = 'gamecontrollerdb.txt'
const createMockStage = () => {
  const adapter = {
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
    }
  }
  const scene = {
    [$emit]: sinon.stub()
  }

  return {
    adapter,
    scene,
    [$adapter]: adapter,
    [$emit]: sinon.stub(),
    [$scene]: scene
  }
}
const assertCallbacksRegistered = (stageAdapter) => {
  assert.containsAllKeys(stageAdapter.callbacks, [
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
  })
  afterEach(() => {
    stage = null
    inputManager = null
  })
  describe('$init()', () => {
    it('should initialize the InputManager', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      assert.isTrue(inputManager.enabled)
      assert.isOk(inputManager.keyboard)
      assertCallbacksRegistered(stage.adapter)
    })
  })
  describe('$attach()', () => {
    it('should ', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      inputManager[$detach]()
      inputManager[$attach]()
      assert.isTrue(inputManager.enabled)
      assert.isOk(inputManager.keyboard)
      assertCallbacksRegistered(stage.adapter)
    })
  })
  describe('$detach()', () => {
    it('should unregister callbacks', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      inputManager[$detach]()
      assert.isTrue(inputManager.enabled)
      assert.isOk(inputManager.keyboard)
      assert.lengthOf(stage.adapter.callbacks, 0)
    })
  })
  describe('keyboard', () => {
    it('should return input manager keyboard instance', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      assert.notStrictEqual(inputManager.keyboard, stage.adapter.keyboard)
    })
  })
  describe('gamepads', () => {
    it('should return gamepads retrieved from adapter', () => {
      assert.lengthOf(inputManager.gamepads, 0)
      stage.adapter.gamepads = [{}]
      inputManager[$init](stage.adapter, gameControllerDb)
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
      inputManager[$init](stage.adapter, gameControllerDb)
      stage.adapter.callbacks.get('axismotion')({}, 0, 1)
      // assert.isTrue(stage.scene[$bubble].calledOnce)
      // assert.isTrue(stage.scene[$capture].notCalled)
    })
  })
  describe('buttondown callback', () => {
    it('should bubble unwrapped event', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      stage.adapter.callbacks.get('buttondown')({}, 0)
      // assert.isTrue(stage.scene[$bubble].calledOnce)
      // assert.isTrue(stage.scene[$capture].notCalled)
    })
  })
  describe('buttonup callback', () => {
    it('should bubble unwrapped event', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      stage.adapter.callbacks.get('buttonup')({}, 0)
      // assert.isTrue(stage.scene[$bubble].calledOnce)
    })
  })
  describe('connected callback', () => {
    it('should emit connected event', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      stage.adapter.callbacks.get('connected')({})
      assert.isTrue(stage[$emit].calledOnce)
    })
  })
  describe('disconnected callback', () => {
    it('should emit disconnected event', () => {
      inputManager[$init](stage.adapter, gameControllerDb)
      stage.adapter.callbacks.get('disconnected')({})
      // assert.isTrue(stage[$emit].calledOnce)
    })
  })
  describe('keydown callback', () => {
    it('should bubble unmapped event', () => {
      // disable keyboard input mapping
      inputManager.setMapping(inputManager.keyboard.uuid, null)

      inputManager[$init](stage.adapter, gameControllerDb)
      stage.adapter.callbacks.get('keydown')(new Keyboard(), 1, false)
      // assert.isTrue(stage.scene[$bubble].calledOnce)
      // must return the input manager keyboard instance, not the native keyboard
      // assert.strictEqual(stage.scene[$bubble].getCall(0).args[0].device, inputManager.keyboard)
      // assert.isTrue(stage.scene[$capture].notCalled)
    })
  })
  describe('keyup callback', () => {
    it('should bubble unwrapped event', () => {
      // disable keyboard input mapping
      inputManager.setMapping(inputManager.keyboard.uuid, null)

      inputManager[$init](stage.adapter, gameControllerDb)
      stage.adapter.callbacks.get('keyup')(new Keyboard(), 1)
      // assert.isTrue(stage.scene[$bubble].calledOnce)
      // must return the input manager keyboard instance, not the native keyboard
      // assert.strictEqual(stage.scene[$bubble].getCall(0).args[0].device, inputManager.keyboard)
    })
  })
})
