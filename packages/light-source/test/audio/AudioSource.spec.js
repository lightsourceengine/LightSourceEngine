/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { AudioSource } from '../../src/audio/AudioSource'
import {
  AudioSourceCapabilityFadeIn,
  AudioSourceCapabilityLoop,
  AudioSourceCapabilityVolume,
  AudioSourceStateError,
  AudioSourceStateLoading,
  AudioSourceStateReady
} from '../../src/audio/constants'
import { $source, $state } from '../../src/util/InternalSymbols'
import sinon from 'sinon'

const testOptions = { id: 'test', type: 'stream' }

describe('AudioSource', () => {
  let audioSource
  beforeEach(() => {
    audioSource = new AudioSource({}, testOptions)
  })
  afterEach(() => {
    audioSource = null
  })
  describe('constructor', () => {
    it('should initialize a new AudioSource', () => {
      assert.isFalse(audioSource.loading)
      assert.isFalse(audioSource.ready)
      assert.isFalse(audioSource.error)
      assert.equal(audioSource.id, 'test')
      assert.equal(audioSource.type, 'stream')
    })
  })
  describe('id', () => {
    it('should return audio source id', () => {
      assert.equal(audioSource.id, 'test')
    })
  })
  describe('type', () => {
    it('should return audio source type', () => {
      assert.equal(audioSource.type, 'stream')
    })
  })
  describe('ready', () => {
    it('should return true when in ready state', () => {
      audioSource[$state] = AudioSourceStateReady
      assert.isTrue(audioSource.ready)
    })
  })
  describe('loading', () => {
    it('should return true when in loading state', () => {
      audioSource[$state] = AudioSourceStateLoading
      assert.isTrue(audioSource.loading)
    })
  })
  describe('error', () => {
    it('should return true when in error state', () => {
      audioSource[$state] = AudioSourceStateError
      assert.isTrue(audioSource.error)
    })
  })
  describe('volume', () => {
    it('should get volume from native source', () => {
      audioSource[$source] = { volume: 1 }
      assert.equal(audioSource.volume, 1)
    })
    it('should set volume of native source', () => {
      audioSource[$source] = { volume: 0 }
      assert.equal(audioSource.volume, 0)
      audioSource.volume = 1
      assert.equal(audioSource.volume, 1)
    })
    it('should constrain value to 0-1 range (upper bound)', () => {
      audioSource[$source] = { volume: 0 }
      audioSource.volume = 1.5
      assert.equal(audioSource.volume, 1)
    })
    it('should constrain value to 0-1 range (lower bound)', () => {
      audioSource[$source] = { volume: 0 }
      audioSource.volume = -1
      assert.equal(audioSource.volume, 0)
    })
  })
  describe('canLoop', () => {
    it('should return native dest capability state', () => {
      audioSource[$source] = { hasCapability (which) { return which === AudioSourceCapabilityLoop } }
      assert.isTrue(audioSource.canLoop)
    })
  })
  describe('canFadeIn', () => {
    it('should return native dest capability state', () => {
      audioSource[$source] = { hasCapability (which) { return which === AudioSourceCapabilityFadeIn } }
      assert.isTrue(audioSource.canFadeIn)
    })
  })
  describe('hasVolume', () => {
    it('should return native dest capability state', () => {
      audioSource[$source] = { hasCapability (which) { return which === AudioSourceCapabilityVolume } }
      assert.isTrue(audioSource.hasVolume)
    })
  })
  describe('play()', () => {
    it('should call native destination play', () => {
      const mock = audioSource[$source] = { play: sinon.stub() }
      audioSource.play()
      assert.isTrue(mock.play.called)
    })
  })
})
