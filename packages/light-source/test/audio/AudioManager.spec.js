/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { $destroy, $init } from '../../src/util/InternalSymbols'
import { AudioManager } from '../../src/audio/AudioManager'
import bindings from 'bindings'

const testWavFile = 'test/resources/test.wav'

describe('AudioManager', () => {
  let audio
  beforeEach(() => {
    audio = new AudioManager({ resourcePath: '' })
    audio[$init](bindings('light-source-ref-audio').createInstance())
  })
  afterEach(() => {
    audio[$destroy]()
  })
  describe('sample', () => {
    it('should be available', () => {
      assert.isTrue(audio.sample.available)
    })
  })
  describe('stream', () => {
    it('should be available', () => {
      assert.isTrue(audio.stream.available)
    })
  })
  describe('devices', () => {
    it('should return Reference audio device name', () => {
      assert.lengthOf(audio.devices, 1)
      assert.include(audio.devices, 'Reference')
    })
  })
  describe('all()', () => {
    it('should return an empty array when no audio sources are loaded', () => {
      assert.lengthOf(audio.all(), 0)
    })
    it('should return all loaded audio sources', () => {
      const audioSource = audio.addAudioSource({ uri: testWavFile, sync: true })
      assert.lengthOf(audio.all(), 1)
      assert.include(audio.all(), audioSource)
    })
    it('should update when audio source removes itself', () => {
      const audioSource = audio.addAudioSource({ uri: testWavFile, sync: true })
      assert.lengthOf(audio.all(), 1)
      audioSource.remove()
      assert.lengthOf(audio.all(), 0)
    })
  })
  describe('addAudioSource()', () => {
    it('should create a new sample audio source synchronously', () => {
      const audioSource = audio.addAudioSource({ uri: testWavFile, sync: true })
      assert.equal(audioSource.id, testWavFile)
      assert.equal(audioSource.type, 'sample')
      assert.isTrue(audioSource.ready)
    })
    it('should create a new stream audio source synchronously', () => {
      const audioSource = audio.addAudioSource({ uri: testWavFile, type: 'stream', sync: true })
      assert.equal(audioSource.id, testWavFile)
      assert.equal(audioSource.type, 'stream')
      assert.isTrue(audioSource.ready)
    })
    // TODO: add async tests
  })
  describe('getAudioSource()', () => {
    it('should return null if id does not exist', () => {
      assert.isNull(audio.getAudioSource('unknown'))
    })
    it('should return audio source matching id', () => {
      const audioSource = audio.addAudioSource({ uri: testWavFile, sync: true })

      assert.equal(audio.getAudioSource(testWavFile), audioSource)
    })
  })
})
