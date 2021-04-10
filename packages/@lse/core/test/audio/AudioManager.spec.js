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
