/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { InputManager, InputManager$attach, InputManager$detach } from '../../src/input/InputManager'
import { $adapter, $bubble, $capture, $events, $scene } from '../../src/util/InternalSymbols'
import sinon from 'sinon'
import { KeyMapping } from '../../src/input/KeyMapping'

const csv = '03000000c82d00002038000000000000,8bitdo,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,' +
  'guide:b2,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,' +
  'righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b4,y:b3,platform:Windows,'

class MockAdapter {
  keyboard = {}
  gamepads = []
  callbacks = new Map()

  getKeyboard () {
    return this.keyboard
  }

  getGamepads () {
    return this.gamepads
  }

  setCallback (type, callback) {
    this.callbacks.set(type, callback)
  }
}

const createMockStage = () => {
  const adapter = new MockAdapter()
  const events = {
    emit: sinon.stub()
  }
  const scene = {
    [$bubble]: sinon.stub(),
    [$capture]: sinon.stub()
  }

  return {
    adapter,
    events,
    scene,
    [$adapter]: adapter,
    [$events]: events,
    [$scene]: scene
  }
}

describe('InputManager', () => {
  let inputManager
  let stage
  beforeEach(() => {
    inputManager = new InputManager()
    stage = createMockStage()
  })
  afterEach(() => {
    stage = null
    inputManager = null
  })
  describe('constructor', () => {
    it('should add a keyboard mapping', () => {
      assert.isOk(inputManager.getKeyMapping('keyboard'))
    })
  })
  describe('InputManager$attach()', () => {
    it('should register callbacks', () => {
      InputManager$attach(inputManager, stage)
      assert.containsAllKeys(stage.adapter.callbacks, [
        'axismotion',
        'buttondown',
        'buttonup',
        'connected',
        'disconnected',
        'keydown',
        'keyup'
      ])
    })
  })
  describe('InputManager$detach()', () => {
    it('should clear adapter and unregister callbacks', () => {
      stage.adapter.resetCallbacks = sinon.stub()

      InputManager$attach(inputManager, stage)
      InputManager$detach(inputManager)

      assert.isNull(inputManager[$adapter])
      assert.isTrue(stage.adapter.resetCallbacks.called)
    })
  })
  describe('keyboard', () => {
    it('should return keyboard retrieved from adapter', () => {
      assert.isNull(inputManager.keyboard)
      InputManager$attach(inputManager, stage)
      assert.strictEqual(inputManager.keyboard, stage.adapter.keyboard)
    })
  })
  describe('gamepads', () => {
    it('should return gamepads retrieved from adapter', () => {
      assert.lengthOf(inputManager.gamepads, 0)
      stage.adapter.gamepads = [{}]
      InputManager$attach(inputManager, stage)
      assert.lengthOf(inputManager.gamepads, 1)
    })
  })
  describe('getKeyMapping()', () => {
    it('should return null for unregistered uuid', () => {
      assert.isNull(inputManager.getKeyMapping('unknown'))
    })
  })
  describe('addKeyMapping()', () => {
    it('should add a new key mapping by uuid', () => {
      const keyMapping = new KeyMapping('standard', [])

      inputManager.addKeyMapping('testUUID', keyMapping)

      assert.strictEqual(inputManager.getKeyMapping('testUUID'), keyMapping)
    })
    it('should throw Error for invalid uuid', () => {
      for (const input of [undefined, null, '', 0, {}]) {
        assert.throws(() => inputManager.addKeyMapping(input, new KeyMapping('standard', [])))
      }
    })
    it('should throw Error for invalid key mapping', () => {
      for (const input of [undefined, null, '', 0, {}]) {
        assert.throws(() => inputManager.addKeyMapping('testUUID', input))
      }
    })
  })
  describe('removeKeyMapping()', () => {
    it('should remove key mapping by uuid', () => {
      inputManager.addKeyMapping('testUUID', new KeyMapping('standard', []))
      inputManager.removeKeyMapping('testUUID')
      assert.isNull(inputManager.getKeyMapping('testUUID'))
    })
    it('should be a noop for unregistered uuid', () => {
      inputManager.removeKeyMapping('testUUID')
    })
  })
  describe('addGameControllerMappings()', () => {
    it('should call addGameControllerMappings on adapter', () => {
      stage.adapter.addGameControllerMappings = sinon.stub()
      InputManager$attach(inputManager, stage)
      inputManager.addGameControllerMappings(csv)
      assert.isTrue(stage.adapter.addGameControllerMappings.called)
    })
    it('should throw when addGameControllerMappings is not provided by adapter', () => {
      InputManager$attach(inputManager, stage)
      assert.throws(() => inputManager.addGameControllerMappings(csv))
    })
  })
  describe('axismotion callback', () => {
    it('should bubble unmapped event', () => {
      InputManager$attach(inputManager, stage)
      stage.adapter.callbacks.get('axismotion')({}, 0, 1)
      assert.isTrue(stage.scene[$bubble].called)
      assert.isFalse(stage.scene[$capture].called)
    })
  })
  describe('buttondown callback', () => {
    it('should bubble unwrapped event', () => {
      InputManager$attach(inputManager, stage)
      stage.adapter.callbacks.get('buttondown')({}, 0)
      assert.isTrue(stage.scene[$bubble].called)
      // TODO: assert.isTrue(mockStage[$scene][$capture].called)
    })
  })
  describe('buttonup callback', () => {
    it('should bubble unwrapped event', () => {
      InputManager$attach(inputManager, stage)
      stage.adapter.callbacks.get('buttonup')({}, 0)
      assert.isTrue(stage.scene[$bubble].called)
      // TODO: assert.isTrue(mockStage[$scene][$capture].called)
    })
  })
  describe('connected callback', () => {
    it('should emit connected event', () => {
      InputManager$attach(inputManager, stage)
      stage.adapter.callbacks.get('connected')({})
      assert.isTrue(stage.events.emit.called)
    })
  })
  describe('disconnected callback', () => {
    it('should emit disconnected event', () => {
      InputManager$attach(inputManager, stage)
      stage.adapter.callbacks.get('disconnected')({})
      assert.isTrue(stage.events.emit.called)
    })
  })
  describe('keydown callback', () => {
    it('should bubble unmapped event', () => {
      InputManager$attach(inputManager, stage)
      // TODO: need API to clear default keyboard mapping
      inputManager.addKeyMapping('keyboard', new KeyMapping('test', []))
      stage.adapter.callbacks.get('keydown')({}, 1, false)
      assert.isTrue(stage.scene[$bubble].called)
    })
  })
  describe('keyup callback', () => {
    it('should bubble unwrapped event', () => {
      InputManager$attach(inputManager, stage)
      // TODO: need API to clear default keyboard mapping
      inputManager.addKeyMapping('keyboard', new KeyMapping('test', []))
      stage.adapter.callbacks.get('keyup')({}, 1)
      assert.isTrue(stage.scene[$bubble].called)
    })
  })
})
