/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { StreamAudioSource } from '../../src/audio/AudioSource.js'
import {
  AudioSourceCapabilityFadeIn,
  AudioSourceCapabilityLoop,
  AudioSourceCapabilityVolume,
  AudioSourceStateError,
  AudioSourceStateLoading,
  AudioSourceStateReady
} from '../../src/audio/constants.js'
import sinon from 'sinon'
import { AudioSourceType } from '../../src/audio/AudioSourceType.js'
import { test } from '../test-env.js'
import { EventName } from '../../src/event/EventName.js'

const { assert } = chai
const testWavFile = 'test/resources/test.wav'

describe('AudioSource', () => {
  let audioSource
  beforeEach(() => {
    audioSource = new StreamAudioSource({}, 'test')
  })
  afterEach(() => {
    for (const as of test.stage.audio.all()) {
      test.stage.audio.delete(as.getId())
    }
    audioSource = null
  })
  describe('constructor', () => {
    it('should initialize a new AudioSource', () => {
      assert.isFalse(audioSource.isLoading())
      assert.isFalse(audioSource.isReady())
      assert.isFalse(audioSource.isError())
      assert.equal(audioSource.getId(), 'test')
      assert.equal(audioSource.getType(), AudioSourceType.Stream)
    })
  })
  describe('getId()', () => {
    it('should return audio source id', () => {
      assert.equal(audioSource.getId(), 'test')
    })
  })
  describe('getType()', () => {
    it('should return audio source type', () => {
      assert.equal(audioSource.getType(), AudioSourceType.Stream)
    })
  })
  describe('isReady()', () => {
    it('should return true when in ready state', () => {
      audioSource.$setState(AudioSourceStateReady)
      assert.isTrue(audioSource.isReady())
    })
  })
  describe('isLoading()', () => {
    it('should return true when in loading state', () => {
      audioSource.$setState(AudioSourceStateLoading)
      assert.isTrue(audioSource.isLoading())
    })
  })
  describe('isError()', () => {
    it('should return true when in error state', () => {
      audioSource.$setState(AudioSourceStateError)
      assert.isTrue(audioSource.isError())
    })
  })
  describe('volume', () => {
    it('should get volume from native source', () => {
      audioSource.$setNative({ volume: 1 })
      assert.equal(audioSource.getVolume(), 1)
    })
    it('should set volume of native source', () => {
      audioSource.$setNative({ volume: 0 })
      assert.equal(audioSource.getVolume(), 0)
      audioSource.setVolume(1)
      assert.equal(audioSource.getVolume(), 1)
    })
    it('should constrain value to 0-1 range (upper bound)', () => {
      audioSource.$setNative({ volume: 0 })
      audioSource.setVolume(1.5)
      assert.equal(audioSource.getVolume(), 1)
    })
    it('should constrain value to 0-1 range (lower bound)', () => {
      audioSource.$setNative({ volume: 0 })
      audioSource.setVolume(-1)
      assert.equal(audioSource.getVolume(), 0)
    })
  })
  describe('canLoop()', () => {
    it('should return native dest capability state', () => {
      audioSource.$setNative({ hasCapability (which) { return which === AudioSourceCapabilityLoop } })
      assert.isTrue(audioSource.canLoop())
    })
  })
  describe('canFadeIn()', () => {
    it('should return native dest capability state', () => {
      audioSource.$setNative({ hasCapability (which) { return which === AudioSourceCapabilityFadeIn } })
      assert.isTrue(audioSource.canFadeIn())
    })
  })
  describe('hasVolume()', () => {
    it('should return native dest capability state', () => {
      audioSource.$setNative({ hasCapability (which) { return which === AudioSourceCapabilityVolume } })
      assert.isTrue(audioSource.hasVolume())
    })
  })
  describe('play()', () => {
    it('should call native destination play', () => {
      const mock = { play: sinon.stub() }

      audioSource.$setNative(mock)
      audioSource.play()
      assert.isTrue(mock.play.called)
    })
  })
  describe('status event', () => {
    it('should dispatch isReady() status event for async load', async () => {
      const as = test.stage.audio.addSample(testWavFile)

      await isReadyPromise(as)

      assert.isTrue(as.isReady())
    })
    it('should dispatch isError() status event for async load', async () => {
      const as = test.stage.audio.addSample('invalid')

      await isErrorPromise(as)

      assert.isTrue(as.isError())
    })
    it('should dispatch isReady() status event for sync load', async () => {
      const as = test.stage.audio.addSample(testWavFile, { sync: true })

      assert.isTrue(as.isReady())

      await isReadyPromise(as)

      assert.isTrue(as.isReady())
    })
    it('should dispatch isError() status event for sync load', async () => {
      const as = test.stage.audio.addSample('invalid', { sync: true })

      assert.isTrue(as.isError())

      await isErrorPromise(as)

      assert.isTrue(as.isError())
    })
  })
})

const isReadyPromise = (as) => {
  return new Promise((resolve, reject) => {
    as.once(EventName.onStatus, (as) => {
      if (as.target.isReady()) {
        resolve()
      } else {
        reject(Error('expected isReady() change event'))
      }
    })
  })
}

const isErrorPromise = (as) => {
  return new Promise((resolve, reject) => {
    as.once(EventName.onStatus, (as) => {
      if (as.target.isError()) {
        resolve()
      } else {
        reject(Error('expected isError() change event'))
      }
    })
  })
}
