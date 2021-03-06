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
import { AudioDestination } from '../../src/audio/AudioDestination.mjs'
import {
  AudioDestinationCapabilityFadeOut,
  AudioDestinationCapabilityPause,
  AudioDestinationCapabilityResume,
  AudioDestinationCapabilityStop,
  AudioDestinationCapabilityVolume
} from '../../src/audio/constants.mjs'
import sinon from 'sinon'
import { AudioDecoderType } from '../../src/audio/AudioDecoderType.mjs'
import { AudioType } from '../../src/audio/AudioType.mjs'

const { assert } = chai

describe('AudioDestination', () => {
  let dest
  let plugin
  beforeEach(() => {
    dest = new AudioDestination(AudioType.STREAM)
    plugin = mockPlugin()
    dest.$setPlugin(plugin)
  })
  afterEach(() => {
    dest = null
    plugin = null
  })
  describe('constructor', () => {
    it('should set type', () => {
      assert.equal(dest.type, AudioType.STREAM)
    })
  })
  describe('isAvailable()', () => {
    it('should be false with no native destination', () => {
      dest = new AudioDestination(AudioType.STREAM)
      assert.isFalse(dest.isAvailable())
    })
    it('should be true with native destination', () => {
      assert.isTrue(dest.isAvailable())
    })
  })
  describe('volume', () => {
    it('should get volume from native destination', () => {
      Array.isArray(dest.volume)
      assert.isTrue(plugin.native.getVolume.called)
    })
    it('should set volume of native destination', () => {
      dest.volume = 1
      assert.isTrue(plugin.native.setVolume.calledWith(1))
    })
    it('should constrain value to 0-1 range (upper bound)', () => {
      dest.volume = 10
      assert.isTrue(plugin.native.setVolume.calledWith(1))
    })
    it('should constrain value to 0-1 range (lower bound)', () => {
      dest.volume = -10
      assert.isTrue(plugin.native.setVolume.calledWith(0))
    })
  })
  describe('decoders', () => {
    it('should return decoders from native destination', () => {
      assert.lengthOf(dest.decoders, 1)
      assert.include(dest.decoders, AudioDecoderType.OGG)
    })
  })
  describe('rawDecoders', () => {
    it('should return raw decoders from native destination', () => {
      assert.lengthOf(dest.rawDecoders, 2)
      assert.include(dest.rawDecoders, testDecoder)
      assert.include(dest.rawDecoders, AudioDecoderType.OGG)
    })
  })
  describe('hasDecoder()', () => {
    it('should return true if decoder exists', () => {
      assert.isTrue(dest.hasDecoder(AudioDecoderType.OGG))
    })
  })
  describe('canPause()', () => {
    it('should return native dest capability state', () => {
      plugin.native.capabilities.set(AudioDestinationCapabilityPause, true)
      assert.isTrue(dest.canPause())
    })
  })
  describe('canResume()', () => {
    it('should return native dest capability state', () => {
      plugin.native.capabilities.set(AudioDestinationCapabilityResume, true)
      assert.isTrue(dest.canResume())
    })
  })
  describe('canStop()', () => {
    it('should return native dest capability state', () => {
      plugin.native.capabilities.set(AudioDestinationCapabilityStop, true)
      assert.isTrue(dest.canStop())
    })
  })
  describe('canFadeOut()', () => {
    it('should return native dest capability state', () => {
      plugin.native.capabilities.set(AudioDestinationCapabilityFadeOut, true)
      assert.isTrue(dest.canFadeOut())
    })
  })
  describe('hasVolume()', () => {
    it('should return native dest capability state', () => {
      plugin.native.capabilities.set(AudioDestinationCapabilityVolume, true)
      assert.isTrue(dest.hasVolume())
    })
  })
  describe('pause()', () => {
    it('should call native destination pause', () => {
      dest.pause()
      assert.isTrue(plugin.native.pause.called)
    })
  })
  describe('resume()', () => {
    it('should call native destination resume', () => {
      dest.resume()
      assert.isTrue(plugin.native.resume.called)
    })
  })
  describe('stop()', () => {
    it('should call native destination stop', () => {
      dest.stop()
      assert.isTrue(plugin.native.stop.called)
    })
  })
  describe('all()', () => {
    it('should return no sources', () => {
      assert.lengthOf(dest.all(), 0)
    })
    it('should return 1 source', () => {
      dest.add('file.wav')
      assert.lengthOf(dest.all(), 1)
    })
  })
  describe('get()', () => {
    it('should return undefined if source does not exist', () => {
      assert.isUndefined(dest.get('file.wav'))
    })
    it('should return source for cache key', () => {
      const source = dest.add('file.wav')
      assert.strictEqual(dest.get('file.wav'), source)
    })
  })
  describe('add()', () => {
    it('should add source from filename string', () => {
      dest.add('file.wav')
      assert.lengthOf(dest.all(), 1)
    })
    it('should throw Error for buffer argument', () => {
      assert.throws(() => dest.add(Buffer.from('')))
    })
    it('should add source from uri filename', () => {
      dest.add({ uri: 'file.wav' })
      assert.lengthOf(dest.all(), 1)
    })
    it('should add source from uri buffer', () => {
      const buffer = Buffer.from('')
      dest.add({ uri: buffer, key: 'asset' })
      assert.lengthOf(dest.all(), 1)
      assert.equal(buffer, dest.get('asset').uri)
    })
    it('should add source from data uri', () => {
      dest.add({ uri: `data:audio/audio;base64,${Buffer.from('mock').toString('base64')}` })
      assert.lengthOf(dest.all(), 1)
    })
    it('should use cache key', () => {
      const source = dest.add({ uri: 'file.wav', key: 'asset' })
      assert.lengthOf(dest.all(), 1)
      assert.strictEqual(dest.get('asset'), source)
      assert.isUndefined(dest.get('file.wav'))
    })
    it('should throw if key already exists in cache', () => {
      dest.add('file.wav')
      assert.throws(() => dest.add('file.wav'))
    })
    it('should throw if options invalid type', () => {
      for (const input of [1, [], NaN, true]) {
        assert.throws(() => dest.add(input))
      }
    })
  })
  describe('delete()', () => {
    it('should return source for cache key', () => {
      dest.add('file.wav')
      assert.lengthOf(dest.all(), 1)
      dest.delete('file.wav')
      assert.lengthOf(dest.all(), 0)
    })
  })
  describe('has()', () => {
    it('should return true if key exists', () => {
      dest.add('file.wav')
      assert.isTrue(dest.has('file.wav'))
    })
    it('should return false if key does not exist', () => {
      assert.isFalse(dest.has('file.wav'))
    })
  })
})

const testDecoder = 'TEST'

const mockNativeDestination = () => ({
  capabilities: new Map(),
  getDecoders () {
    return [testDecoder, AudioDecoderType.OGG]
  },
  getVolume: sinon.stub(),
  setVolume: sinon.stub(),
  hasCapability (capability) {
    return this.capabilities.get(capability)
  },
  createAudioSource () {
    return {
      destroy () {
      }
    }
  },
  pause: sinon.stub(),
  resume: sinon.stub(),
  stop: sinon.stub()
})

const mockPlugin = () => ({
  native: null,
  createSampleAudioDestination () {
    this.native = mockNativeDestination()
    return this.native
  },
  createStreamAudioDestination () {
    this.native = mockNativeDestination()
    return this.native
  }
})
