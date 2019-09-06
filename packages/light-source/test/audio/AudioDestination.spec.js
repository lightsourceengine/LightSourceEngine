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

describe('AudioDestination', () => {
  let dest
  beforeEach(() => {
    dest = new AudioDestination()
  })
  afterEach(() => {
    dest = null
  })
  describe('available', () => {
    it('should be false with no native destination', () => {
      assert.isFalse(dest.available)
    })
    it('should be true with native destination', () => {
      dest[$destination] = {}
      assert.isTrue(dest.available)
    })
  })
  describe('volume', () => {
    it('should get volume from native destination', () => {
      dest[$destination] = { volume: 1 }
      assert.equal(dest.volume, 1)
    })
    it('should set volume of native destination', () => {
      dest[$destination] = { volume: 0 }
      assert.equal(dest.volume, 0)
      dest.volume = 1
      assert.equal(dest.volume, 1)
    })
    it('should constrain value to 0-1 range (upper bound)', () => {
      dest[$destination] = { volume: 0 }
      dest.volume = 1.5
      assert.equal(dest.volume, 1)
    })
    it('should constrain value to 0-1 range (lower bound)', () => {
      dest[$destination] = { volume: 0 }
      dest.volume = -1
      assert.equal(dest.volume, 0)
    })
  })
  describe('decoders', () => {
    it('should return decoders from native destination', () => {
      dest[$destination] = { decoders: ['MOCK'] }
      assert.lengthOf(dest.decoders, 1)
      assert.include(dest.decoders, 'MOCK')
    })
  })
  describe('canPause', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityPause } }
      assert.isTrue(dest.canPause)
    })
  })
  describe('canResume', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityResume } }
      assert.isTrue(dest.canResume)
    })
  })
  describe('canStop', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityStop } }
      assert.isTrue(dest.canStop)
    })
  })
  describe('canFadeOut', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityFadeOut } }
      assert.isTrue(dest.canFadeOut)
    })
  })
  describe('hasVolume', () => {
    it('should return native dest capability state', () => {
      dest[$destination] = { hasCapability (which) { return which === AudioDestinationCapabilityVolume } }
      assert.isTrue(dest.hasVolume)
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
