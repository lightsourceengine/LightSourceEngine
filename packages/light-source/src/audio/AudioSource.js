/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import {
  $plugin, $asyncId, $audioSourceMap,
  $buffer,
  $destination,
  $options,
  $owner,
  $resourcePath,
  $source,
  $state
} from '../util/InternalSymbols'
import { clamp, isNumber, resolveUri } from '../util'
import { readFileSync, promises } from 'fs'
import {
  AudioSourceCapabilityFadeIn,
  AudioSourceCapabilityLoop,
  AudioSourceCapabilityVolume,
  AudioSourceStateError,
  AudioSourceStateInit,
  AudioSourceStateLoading,
  AudioSourceStateReady
} from './constants'

let nextAsyncId = 1

const { readFile } = promises

/**
 * An audio resource that can be rendered to a destination output buffer.
 */
export class AudioSource {
  constructor (audio, options) {
    this[$options] = options
    this[$state] = AudioSourceStateInit
    this[$source] = null
    this[$buffer] = null
    this[$asyncId] = 0
    this[$owner] = audio
  }

  /**
   * @returns {string} AudioManager resource ID.
   */
  get id () {
    return this[$options].id
  }

  /**
   * The destination type.
   *
   * @returns {('sample'|'stream')}
   */
  get type () {
    return this[$options].type
  }

  /**
   * Is the audio resource ready to be played?
   *
   * @returns {boolean}
   */
  get ready () {
    return this[$state] === AudioSourceStateReady
  }

  /**
   * Is this audio resource loading?
   *
   * If loading, play() should not be called.
   *
   * @returns {boolean}
   */
  get loading () {
    return this[$state] === AudioSourceStateLoading
  }

  /**
   * Was there an error during audio resource loading?
   *
   * @returns {boolean}
   */
  get error () {
    return this[$state] === AudioSourceStateError
  }

  /**
   * Remove the resource from the AudioManager.
   */
  remove () {
    this[$owner][$audioSourceMap].delete(this.id)
    AudioSource$reset(this)
    this[$buffer] = null
  }

  /**
   * Start playback of the source from the beginning.
   *
   * @param {Number} [opts.loops=1] The number of times to repeat playback. If 0, playback will loop foreever.
   * @param {Number} [opts.fadeInMs=0] The time in milliseconds to fade in (volume) playback.
   */
  play (opts = {}) {
    this[$source].play(opts)
  }

  /**
   * The volume of the source.
   *
   * @property volume
   */
  get volume () {
    return this[$source].volume
  }

  set volume (value) {
    this[$source].volume = isNumber(value) ? clamp(value, 0, 1) : 0
  }

  /**
   * Does this source support volume control?
   *
   * @returns {boolean}
   */
  get hasVolume () {
    return this[$source].hasCapability(AudioSourceCapabilityVolume)
  }

  /**
   * Can playback loop?
   *
   * @returns {boolean}
   */
  get canLoop () {
    return this[$source].hasCapability(AudioSourceCapabilityLoop)
  }

  /**
   * Can playback fade in?
   *
   * @returns {boolean}
   */
  get canFadeIn () {
    return this[$source].hasCapability(AudioSourceCapabilityFadeIn)
  }
}

const AudioSource$applyBuffer = (self, buffer) => {
  const audio = self[$owner]
  const adapter = audio[$plugin]

  self[$buffer] = buffer

  if (!self[$source]) {
    if (!adapter.attached) {
      return
    }

    const dest = audio[self[$options].type]

    if (!dest || !dest.available) {
      AudioSource$setErrorState(self, `AudioSource type ${self[$options].type} is unavailable.`)
      return
    }

    try {
      self[$source] = dest[$destination].createAudioSource()
    } catch (e) {
      AudioSource$setErrorState(self, `Failed to create native audio source. Error: ${e.message}`)
      return
    }
  }

  try {
    self[$source].load(buffer)
  } catch (e) {
    AudioSource$setErrorState(self, e.message)
    return
  }

  self[$state] = AudioSourceStateReady
}

export const AudioSource$setErrorState = (self, message) => {
  message && console.log(message)
  AudioSource$reset(self)
  self[$state] = AudioSourceStateError
  self[$buffer] = null
}

export const AudioSource$load = (self, canLoadSync) => {
  switch (self[$state]) {
    case AudioSourceStateReady:
      return
    case AudioSourceStateLoading:
      self[$buffer] && AudioSource$applyBuffer(self, self[$buffer])
      return
    case AudioSourceStateInit:
      if (self[$buffer]) {
        AudioSource$applyBuffer(self, self[$buffer])
        return
      }
      break
  }

  const { uri, sync } = self[$options]
  const file = resolveUri(uri, self[$resourcePath])

  self[$state] = AudioSourceStateLoading

  if (canLoadSync && sync) {
    let buffer

    try {
      buffer = readFileSync(file)
    } catch (e) {
      AudioSource$setErrorState(self, e.message)
      return
    }

    AudioSource$applyBuffer(self, buffer)
  } else {
    const asyncId = self[$asyncId] = nextAsyncId++

    readFile(file)
      .then((buffer) => {
        if (self[$asyncId] !== asyncId ||
          self[$owner].getAudioSource(self.id) !== self ||
          self[$state] !== AudioSourceStateLoading) {
          return
        }

        AudioSource$applyBuffer(self, buffer)
      })
      .catch((e) => {
        AudioSource$setErrorState(self, e.message)
      })
  }
}

export const AudioSource$reset = (self) => {
  self[$state] = AudioSourceStateInit
  self[$asyncId] = 0
  self[$source] && self[$source].destroy()
  self[$source] = null
}
