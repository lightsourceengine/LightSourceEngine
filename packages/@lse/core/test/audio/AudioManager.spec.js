/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { AudioManager } from '../../src/audio/AudioManager.js'
import { loadPlugin } from '../../src/addon/loadPlugin.js'
import { EventName } from '../../src/event/EventName.js'
import { PluginId } from '../../src/addon/PluginId.js'

const { assert } = chai

describe('AudioManager', () => {
  let audio
  beforeEach(() => {
    const mockStage = {}
    audio = new AudioManager(mockStage)
    audio.$setPlugin(loadPlugin(PluginId.REF_AUDIO))
    audio.$attach()
  })
  afterEach(() => {
    audio.$destroy()
    audio = null
  })
  describe('property: devices', () => {
    it('should return Reference audio device name', () => {
      assert.lengthOf(audio.devices, 1)
      assert.include(audio.devices, 'Reference')
    })
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
})

const attachedEventPromise = (audio) => {
  return new Promise((resolve, reject) => {
    audio.on(EventName.attached, () => resolve())
  })
}

const detachedEventPromise = (audio) => {
  return new Promise((resolve, reject) => {
    audio.on(EventName.detached, () => resolve())
  })
}
