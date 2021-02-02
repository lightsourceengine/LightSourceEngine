/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { AudioManager } from '../../src/audio/AudioManager.js'
import { loadPluginById } from '../../src/addon/index.js'
import { AudioSourceType } from '../../src/audio/AudioSourceType.js'
import { EventName } from '../../src/event/EventName.js'

const { assert } = chai
const testWavFile = 'test/resources/test.wav'

describe('AudioManager', () => {
  let audio
  beforeEach(() => {
    const mockStage = {}
    audio = new AudioManager(mockStage)
    audio.$setPlugin(loadPluginById('plugin:ref-audio').createInstance())
    audio.$attach()
  })
  afterEach(() => {
    audio.$destroy()
    audio = null
  })
  describe('attached event', () => {
    it('should emit attached event', async () => {
      audio.$detach()

      const p = attachedEventPromise(audio)

      audio.$attach()

      await p
    })
  })
  describe('detached event', () => {
    it('should emit detached event', async () => {
      const p = detachedEventPromise(audio)

      audio.$detach()

      await p
    })
  })
  describe('isAttached()', () => {
    it('should be attached', () => {
      assert.isTrue(audio.isAttached())
    })
    it('should be detached', () => {
      audio.$detach()
      assert.isFalse(audio.isAttached())
    })
  })
  describe('sample', () => {
    it('should be available', () => {
      assert.isTrue(audio.sample.isAvailable())
    })
  })
  describe('stream', () => {
    it('should be available', () => {
      assert.isTrue(audio.stream.isAvailable())
    })
  })
  describe('getDevices()', () => {
    it('should return Reference audio device name', () => {
      assert.lengthOf(audio.getDevices(), 1)
      assert.include(audio.getDevices(), 'Reference')
    })
  })
  describe('all()', () => {
    it('should return an empty array when no audio sources are loaded', () => {
      assert.lengthOf(audio.all(), 0)
    })
    it('should return all loaded audio sources', () => {
      const audioSource = audio.addSample(testWavFile, { sync: true })
      assert.lengthOf(audio.all(), 1)
      assert.include(audio.all(), audioSource)
    })
    it('should update when audio source is removed', () => {
      const audioSource = audio.addSample(testWavFile, { sync: true })
      assert.lengthOf(audio.all(), 1)
      audio.delete(audioSource.getId())
      assert.lengthOf(audio.all(), 0)
    })
  })
  describe('addSample()', () => {
    it('should create a new sample audio source synchronously', () => {
      const audioSource = audio.addSample(testWavFile, { sync: true })
      assert.equal(audioSource.getId(), testWavFile)
      assert.equal(audioSource.getType(), AudioSourceType.Sample)
      assert.isTrue(audioSource.isReady())
    })
    it('should create a new sample audio source asynchronously', () => {
      const audioSource = audio.addSample(testWavFile)
      assert.equal(audioSource.getId(), testWavFile)
      assert.equal(audioSource.getType(), AudioSourceType.Sample)
      assert.isTrue(audioSource.isLoading())
    })
  })
  describe('addStream()', () => {
    it('should create a new stream audio source synchronously', () => {
      const audioSource = audio.addStream(testWavFile, { sync: true })
      assert.equal(audioSource.getId(), testWavFile)
      assert.equal(audioSource.getType(), AudioSourceType.Stream)
      assert.isTrue(audioSource.isReady())
    })
    it('should create a new stream audio source asynchronously', () => {
      const audioSource = audio.addStream(testWavFile)
      assert.equal(audioSource.getId(), testWavFile)
      assert.equal(audioSource.getType(), AudioSourceType.Stream)
      assert.isTrue(audioSource.isLoading())
    })
  })
  describe('get()', () => {
    it('should return Null-type AudioSource if id does not exist', () => {
      assert.equal(audio.get('unknown').getType(), AudioSourceType.Null)
    })
    it('should return audio source matching id', () => {
      const audioSource = audio.addSample(testWavFile, { sync: true })

      assert.equal(audio.get(testWavFile), audioSource)
    })
  })
  describe('has()', () => {
    it('should return true for registered audio source', () => {
      const audioSource = audio.addSample(testWavFile, { sync: true })

      assert.isTrue(audio.has(audioSource.getId()))
    })
    it('should return false for unregistered audio source', () => {
      assert.isFalse(audio.has('unknown'))
    })
  })
  describe('delete()', () => {
    it('should remove audio source', () => {
      const audioSource = audio.addSample(testWavFile, { sync: true })

      assert.strictEqual(audio.get(audioSource.getId()), audioSource)

      audio.delete(audioSource.getId())

      assert.equal(audio.get(audioSource.getId()).getType(), AudioSourceType.Null)
    })
    it('should be a no-op for unregistered id', () => {
      audio.delete('unknown')
    })
  })
})

const attachedEventPromise = (audio) => {
  return new Promise((resolve, reject) => {
    audio.on(EventName.onAttached, () => resolve())
  })
}

const detachedEventPromise = (audio) => {
  return new Promise((resolve, reject) => {
    audio.on(EventName.onDetached, () => resolve())
  })
}
