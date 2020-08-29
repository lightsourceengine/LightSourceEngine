/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { clamp, isNumber, EventEmitter } from '../util'
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
import { AudioSourceType } from './AudioSourceType'
import { ReadyStatusEvent, ErrorStatusEvent, EventNames } from '../event'

let nextAsyncId = 1
const { readFile } = promises

/**
 * An audio resource that can be rendered to a destination output buffer.
 */
export class AudioSource {
  _audioManager = null
  _id = ''
  _state = AudioSourceStateInit
  _native = null
  _buffer = null
  _asyncId = 0
  _emitter = new EventEmitter([EventNames.status])

  constructor (audio, id) {
    this._audioManager = audio
    this._id = id
  }

  on (name, callback) {
    this._emitter.on(name, callback)
  }

  once (name, callback) {
    this._emitter.once(name, callback)
  }

  off (name, callback) {
    this._emitter.off(name, callback)
  }

  /**
   * @returns {string}
   */
  getType () {
    return ''
  }

  /**
   * @returns {string} AudioManager resource ID.
   */
  getId () {
    return this._id
  }

  /**
   * @returns {boolean} true if the audio source is ready for use (playback, etc); otherwise, false
   */
  isReady () {
    return this._state === AudioSourceStateReady
  }

  /**
   * @returns {boolean} true if the audio source is currently being loaded; otherwise, false
   */
  isLoading () {
    return this._state === AudioSourceStateLoading
  }

  /**
   * @returns {boolean} true if the audio source failed to load it's file; otherwise, false
   */
  isError () {
    return this._state === AudioSourceStateError
  }

  /**
   * Start playback of the source from the beginning.
   *
   * @param {Number} [opts.loops=1] The number of times to repeat playback. If 0, playback will loop foreever.
   * @param {Number} [opts.fadeInMs=0] The time in milliseconds to fade in (volume) playback.
   */
  play (opts = {}) {
    this._native.play(opts)
  }

  /**
   * @returns volume {number} Current value [0-1] of the volume of the audio source.
   */
  getVolume () {
    return this._native.volume
  }

  /**
   * Sets the volume.
   *
   * @param value {number} A value between [0-1]. If the value is not a number, volume is set to 0. If the value is
   * out of range, it is Math.clamp()'d to [0-1].
   */
  setVolume (value) {
    this._native.volume = isNumber(value) ? clamp(value, 0, 1) : 0
  }

  /**
   * @returns {boolean} true if this audio source supports volume controls; otherwise, false
   */
  hasVolume () {
    return this._native.hasCapability(AudioSourceCapabilityVolume)
  }

  /**
   * @returns {boolean} true if this audio source supports the loop option of play(); otherwise, false
   */
  canLoop () {
    return this._native.hasCapability(AudioSourceCapabilityLoop)
  }

  /**
   * @returns {boolean} true if this audio source supports fadeIn; otherwise, false
   */
  canFadeIn () {
    return this._native.hasCapability(AudioSourceCapabilityFadeIn)
  }

  /**
   * @ignore
   */
  $load (sync) {
    switch (this._state) {
      case AudioSourceStateInit:
        if (this._buffer) {
          this._setBuffer(this._buffer, true)
          return
        }
        break
      case AudioSourceStateLoading:
        this._buffer && this._setBuffer(this._buffer, true)
        return
      case AudioSourceStateError:
        break
      default:
        throw Error('AudioSource must be in init or error state to load')
    }

    const path = parseUri(this._id)

    if (sync) {
      let buffer

      try {
        buffer = readFileSync(path)
      } catch (e) {
        this._setError(e, true)
        return
      }

      this._setBuffer(buffer, true)
    } else {
      const asyncId = this._asyncId = nextAsyncId++
      const cancelled = () => this._asyncId !== asyncId || this._state !== AudioSourceStateLoading

      this._state = AudioSourceStateLoading

      readFile(path)
        .then((buffer) => {
          cancelled() || this._setBuffer(buffer, false)
        })
        .catch((e) => {
          cancelled() || this._setError(e, false)
        })
    }
  }

  /**
   * @ignore
   */
  $unload () {
    this._state = AudioSourceStateInit
    this._asyncId = 0
    this._native?.destroy()
    this._native = null
  }

  /**
   * @ignore
   */
  $setNative (nativeAudioSource) {
    this._native = nativeAudioSource
  }

  /**
   * @ignore
   */
  $setState (state) {
    this._state = state
  }

  /**
   * @ignore
   */
  _setError (error, deferred) {
    const message = typeof error === 'string' ? error : error.message

    this.$unload()
    this._state = AudioSourceStateError
    this._buffer = null

    this._emitter.emitEvent(ErrorStatusEvent(this, message), deferred)
  }

  /**
   * @ignore
   */
  _setBuffer (buffer, deferred) {
    const audioManager = this._audioManager

    this._buffer = buffer

    if (!this._native) {
      if (!audioManager.isAttached()) {
        return
      }

      const dest = audioManager[this.getType()]

      if (!dest || !dest.isAvailable()) {
        this._setError(`${this.getType()} type is not loadable for this audio plugin.`, deferred)
        return
      }

      try {
        this._native = dest.$createNativeAudioSource()
      } catch (e) {
        this._setError(e, deferred)
        return
      }
    }

    try {
      this._native.load(buffer)
    } catch (e) {
      this._setError(e, deferred)
      return
    }

    this._state = AudioSourceStateReady

    this._emitter.emitEvent(ReadyStatusEvent(this), deferred)
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
