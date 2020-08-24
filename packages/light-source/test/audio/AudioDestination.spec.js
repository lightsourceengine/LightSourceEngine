/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { AudioDestination } from '../../src/audio/AudioDestination'
import { $destination } from '../../src/util/InternalSymbols'
import {
  AudioDestinationCapabilityFadeOut,
  AudioDestinationCapabilityPause,
  AudioDestinationCapabilityResume,
  AudioDestinationCapabilityStop,
  AudioDestinationCapabilityVolume
} from '../../src/audio/constants'
import sinon from 'sinon'
import { AudioDecoder } from '../../src/audio/AudioDecoder'

const $setNativeDestination = Symbol.for('setNativeDestination')
const kTestDecoders = [AudioDecoder.OGG, 'MOCK']

describe('AudioDestination', () => {
  let dest
  beforeEach(() => {
    dest = new AudioDestination()
  })
  afterEach(() => {
    dest = null
  })
  describe('isAvailable()', () => {
    it('should be false with no native destination', () => {
      assert.isFalse(dest.isAvailable())
    })
    it('should be true with native destination', () => {
      dest[$destination] = {}
      assert.isTrue(dest.isAvailable())
    })
  })
  describe('volume', () => {
    it('should get volume from native destination', () => {
      dest[$destination] = { volume: 1 }
      assert.equal(dest.getVolume(), 1)
    })
    it('should set volume of native destination', () => {
      dest[$destination] = { volume: 0 }
      assert.equal(dest.getVolume(), 0)
      dest.setVolume(1)
      assert.equal(dest.getVolume(), 1)
    })
    it('should constrain value to 0-1 range (upper bound)', () => {
      dest[$destination] = { volume: 0 }
      dest.setVolume(1.5)
      assert.equal(dest.getVolume(), 1)
    })
    it('should constrain value to 0-1 range (lower bound)', () => {
      dest[$destination] = { volume: 0 }
      dest.setVolume(-1)
      assert.equal(dest.getVolume(), 0)
    })
  })
  describe('getDecoders()', () => {
    it('should return decoders from native destination', () => {
      dest[$setNativeDestination]({ decoders: kTestDecoders })
      assert.lengthOf(dest.getDecoders(), 1)
      assert.include(dest.getDecoders(), AudioDecoder.OGG)
    })
  })
  describe('getRawDecoders()', () => {
    it('should return raw decoders from native destination', () => {
      dest[$setNativeDestination]({ decoders: kTestDecoders })
      assert.lengthOf(dest.getRawDecoders(), 2)
      assert.include(dest.getRawDecoders(), AudioDecoder.OGG)
      assert.include(dest.getRawDecoders(), 'MOCK')
    })
  })
  describe('hasDecoder()', () => {
    it('should return true if decoder exists', () => {
      dest[$setNativeDestination]({ decoders: kTestDecoders })

      assert.isTrue(dest.hasDecoder(AudioDecoder.OGG))
      assert.isTrue(dest.hasDecoder('MOCK'))
    })
  })
  describe('canPause()', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityPause } }
      assert.isTrue(dest.canPause())
    })
  })
  describe('canResume()', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityResume } }
      assert.isTrue(dest.canResume())
    })
  })
  describe('canStop()', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityStop } }
      assert.isTrue(dest.canStop())
    })
  })
  describe('canFadeOut()', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityFadeOut } }
      assert.isTrue(dest.canFadeOut())
    })
  })
  describe('hasVolume()', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityVolume } }
      assert.isTrue(dest.hasVolume())
    })
  })
  describe('pause()', () => {
    it('should call native destination pause', () => {
      const mock = dest[$destination] = { pause: sinon.stub() }
      dest.pause()
      assert.isTrue(mock.pause.called)
    })
  })
  describe('resume()', () => {
    it('should call native destination resume', () => {
      const mock = dest[$destination] = { resume: sinon.stub() }
      dest.resume()
      assert.isTrue(mock.resume.called)
    })
  })
  describe('stop()', () => {
    it('should call native destination stop', () => {
      const mock = dest[$destination] = { stop: sinon.stub() }
      dest.stop()
      assert.isTrue(mock.stop.called)
    })
  })
})
