/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { AudioSource } from '../../src/audio/AudioSource.js'
import {
  AudioSourceCapabilityFadeIn,
  AudioSourceCapabilityLoop,
  AudioSourceCapabilityVolume
} from '../../src/audio/constants.js'
import sinon from 'sinon'
import { AudioType } from '../../src/audio/AudioType.js'
import { EventName } from '../../src/event/EventName.js'
import { readFileSync } from 'fs'

const { assert } = chai
const testWavFile = 'test/resources/test.wav'
const testAudioType = AudioType.STREAM
const toFontUri = (buffer, mediaType) => `data:audio/${mediaType};base64,${buffer.toString('base64')}`

describe('AudioSource', () => {
  let source
  let mockNativeAudioSource
  beforeEach(() => {
    mockNativeAudioSource = createMockNativeAudioSource()
    source = new AudioSource(mockNativeAudioSource, testAudioType, testWavFile)
  })
  afterEach(() => {
    source = null
    mockNativeAudioSource = null
  })
  describe('constructor', () => {
    it('should initialize a new AudioSource', () => {
      assert.isFalse(source.isLoading())
      assert.isFalse(source.isReady())
      assert.isFalse(source.isError())
      assert.equal(source.uri, testWavFile)
      assert.equal(source.type, testAudioType)
    })
  })
  describe('$load', () => {
    it('should load audio source from file synchronously', async () => {
      return testLoad(testWavFile, true, true, 'isReady', 'isReady')
    })
    it('should load audio source from file asynchronously', async () => {
      return testLoad(testWavFile, false, true, 'isLoading', 'isReady')
    })
    it('should load audio source from buffer synchronously', async () => {
      return testLoad(readFileSync(testWavFile), true, true, 'isReady', 'isReady')
    })
    it('should load audio source from buffer asynchronously', async () => {
      return testLoad(readFileSync(testWavFile), false, true, 'isReady', 'isReady')
    })
    it('should load audio source from buffer synchronously', async () => {
      return testLoad(toFontUri(readFileSync(testWavFile), 'audio'), true, true, 'isReady', 'isReady')
    })
    it('should load audio source from buffer asynchronously', async () => {
      return testLoad(toFontUri(readFileSync(testWavFile), 'audio'), false, true, 'isReady', 'isReady')
    })
    it('should enter error state when file not found', async () => {
      source = new AudioSource(mockNativeAudioSource, testAudioType, 'file-not-found')

      source.$load(false, true)

      assert.isTrue(source.isLoading())

      const event = await getEvent(source)

      assert.isTrue(source.isError())
      assert.equal(event.type, EventName.status)
      assert.isDefined(event.error)
    })
  })
  describe('volume', () => {
    it('should get volume from native source', () => {
      const volume = source.volume

      assert.isUndefined(volume)
      assert.isTrue(mockNativeAudioSource.getVolume.called)
    })
    it('should set volume with native source', () => {
      source.volume = 1
      assert.isTrue(mockNativeAudioSource.setVolume.calledWith(1))
    })
    it('should set volume with native source and clamp +', () => {
      source.volume = 3
      assert.isTrue(mockNativeAudioSource.setVolume.calledWith(1))
    })
    it('should set volume with native source and clamp -', () => {
      source.volume = -2
      assert.isTrue(mockNativeAudioSource.setVolume.calledWith(0))
    })
  })
  describe('canLoop()', () => {
    it('should return native dest capability state', () => {
      mockNativeAudioSource.capabilities.set(AudioSourceCapabilityLoop, true)
      assert.isTrue(source.canLoop())
    })
  })
  describe('canFadeIn()', () => {
    it('should return native dest capability state', () => {
      mockNativeAudioSource.capabilities.set(AudioSourceCapabilityFadeIn, true)
      assert.isTrue(source.canFadeIn())
    })
  })
  describe('hasVolume()', () => {
    it('should return native dest capability state', () => {
      mockNativeAudioSource.capabilities.set(AudioSourceCapabilityVolume, true)
      assert.isTrue(source.hasVolume())
    })
  })
  describe('play()', () => {
    it('should call native destination play', () => {
      source.play()
      assert.isTrue(mockNativeAudioSource.play.called)
    })
  })
  const testLoad = async (uri, sync, defer, stateSync, stateDone) => {
    const source = new AudioSource(mockNativeAudioSource, testAudioType, uri)

    source.$load(sync, defer)

    assert.isTrue(source[stateSync]())

    const event = await getEvent(source)

    assert.isTrue(source[stateDone]())
    assert.equal(event.type, EventName.status)
  }
})

const getEvent = async (source) => {
  return new Promise((resolve, reject) => {
    source.once(EventName.status, resolve)
  })
}

const createMockNativeAudioSource = () => ({
  getVolume: sinon.stub(),
  setVolume: sinon.stub(),
  load: sinon.stub(),
  unload: sinon.stub(),
  destroy: sinon.stub(),
  play: sinon.stub(),
  hasCapability (capability) {
    return this.capabilities.get(capability) ?? false
  },
  capabilities: new Map()
})
