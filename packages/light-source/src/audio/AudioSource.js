/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { clamp, isNumber } from '../util'
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
import { EventEmitter } from 'events'
import { AudioSourceType } from './AudioSourceType'

let nextAsyncId = 1

const { readFile } = promises

const $id = Symbol('id')
const $setBuffer = Symbol('setBuffer')
const $setError = Symbol('setError')
const $asyncId = Symbol('asyncId')
const $buffer = Symbol('buffer')
const $owner = Symbol('owner')
const $emitter = Symbol('emitter')

const $load = Symbol.for('load')
const $unload = Symbol.for('unload')
const $destination = Symbol.for('destination')

// expose for tests
const $state = Symbol.for('state')
const $source = Symbol.for('source')

/**
 * An audio resource that can be rendered to a destination output buffer.
 */
export class AudioSource {
  constructor (audio, id) {
    this[$owner] = audio
    this[$id] = id
    this[$state] = AudioSourceStateInit
    this[$source] = null
    this[$buffer] = null
    this[$asyncId] = 0
    this[$emitter] = new EventEmitter()
  }

  on (name, callback) {
    this[$emitter].on(name, callback)
  }

  once (name, callback) {
    this[$emitter].once(name, callback)
  }

  off (name, callback) {
    this[$emitter].off(name, callback)
  }

  /**
   * @returns {AudioSourceType}
   */
  getType () {
    // sub-class will override
  }

  /**
   * @returns {string} AudioManager resource ID.
   */
  getId () {
    return this[$id]
  }

  /**
   * @returns {boolean} true if the audio source is ready for use (playback, etc); otherwise, false
   */
  isReady () {
    return this[$state] === AudioSourceStateReady
  }

  /**
   * @returns {boolean} true if the audio source is currently being loaded; otherwise, false
   */
  isLoading () {
    return this[$state] === AudioSourceStateLoading
  }

  /**
   * @returns {boolean} true if the audio source failed to load it's file; otherwise, false
   */
  isError () {
    return this[$state] === AudioSourceStateError
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
   * @returns volume {number} Current value [0-1] of the volume of the audio source.
   */
  getVolume () {
    return this[$source].volume
  }

  /**
   * Sets the volume.
   *
   * @param value {number} A value between [0-1]. If the value is not a number, volume is set to 0. If the value is
   * out of range, it is Math.clamp()'d to [0-1].
   */
  setVolume (value) {
    this[$source].volume = isNumber(value) ? clamp(value, 0, 1) : 0
  }

  /**
   * @returns {boolean} true if this audio source supports volume controls; otherwise, false
   */
  hasVolume () {
    return this[$source].hasCapability(AudioSourceCapabilityVolume)
  }

  /**
   * @returns {boolean} true if this audio source supports the loop option of play(); otherwise, false
   */
  canLoop () {
    return this[$source].hasCapability(AudioSourceCapabilityLoop)
  }

  /**
   * @returns {boolean} true if this audio source supports fadeIn; otherwise, false
   */
  canFadeIn () {
    return this[$source].hasCapability(AudioSourceCapabilityFadeIn)
  }

  /**
   * @ignore
   */
  [$load] (sync) {
    switch (this[$state]) {
      case AudioSourceStateInit:
        if (this[$buffer]) {
          this[$setBuffer](this[$buffer], true)
          return
        }
        break
      case AudioSourceStateLoading:
        this[$buffer] && this[$setBuffer](this[$buffer], true)
        return
      case AudioSourceStateError:
        break
      default:
        throw Error('AudioSource must be in init or error state to load')
    }

    const path = parseUri(this[$id])

    if (sync) {
      let buffer

      try {
        buffer = readFileSync(path)
      } catch (e) {
        this[$setError](e, true)
        return
      }

      this[$setBuffer](buffer, true)
    } else {
      const asyncId = this[$asyncId] = nextAsyncId++
      const cancelled = () => this[$asyncId] !== asyncId || this[$state] !== AudioSourceStateLoading

      this[$state] = AudioSourceStateLoading

      readFile(path)
        .then((buffer) => {
          cancelled() || this[$setBuffer](buffer, false)
        })
        .catch((e) => {
          cancelled() || this[$setError](e, false)
        })
    }
  }

  /**
   * @ignore
   */
  [$unload] () {
    this[$state] = AudioSourceStateInit
    this[$asyncId] = 0
    this[$source] && this[$source].destroy()
    this[$source] = null
  }

  /**
   * @ignore
   */
  [$setError] (error, deferred) {
    const message = typeof error === 'string' ? error : error.message

    this[$unload]()
    this[$state] = AudioSourceStateError
    this[$buffer] = null

    if (deferred) {
      queueMicrotask(() => this[$emitter].emit('status', this, message))
    } else {
      this[$emitter].emit('status', this, message)
    }
  }

  /**
   * @ignore
   */
  [$setBuffer] (buffer, deferred) {
    const audio = this[$owner]

    this[$buffer] = buffer

    if (!this[$source]) {
      if (!audio.isAttached()) {
        return
      }

      const dest = audio[this.getType()]

      if (!dest || !dest.isAvailable()) {
        this[$setError](`${this.getType()} type is not loadable for this audio plugin.`, deferred)
        return
      }

      try {
        this[$source] = dest[$destination].createAudioSource()
      } catch (e) {
        this[$setError](e, deferred)
        return
      }
    }

    try {
      this[$source].load(buffer)
    } catch (e) {
      this[$setError](e, deferred)
      return
    }

    this[$state] = AudioSourceStateReady

    if (deferred) {
      queueMicrotask(() => this[$emitter].emit('status', this, null))
    } else {
      this[$emitter].emit('status', this, null)
    }
  }
}

/**
 * @ignore
 */
export class SampleAudioSource extends AudioSource {
  getType () {
    return AudioSourceType.Sample
  }
}

/**
 * @ignore
 */
export class StreamAudioSource extends AudioSource {
  getType () {
    return AudioSourceType.Stream
  }
}

/**
 * @ignore
 */
export class NullAudioSource extends AudioSource {
  getType () {
    return AudioSourceType.Null
  }
}

/**
 * @ignore
 */
const parseUri = (uri) => uri.startsWith('file:') ? uri.substr(5, uri.length - uri.search('?')) : uri
