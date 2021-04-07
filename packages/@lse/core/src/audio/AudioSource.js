/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { clamp, isNumber } from '../util/index.js'
import {
  AudioSourceCapabilityFadeIn,
  AudioSourceCapabilityLoop,
  AudioSourceCapabilityVolume,
  AudioSourceStateError,
  AudioSourceStateInit,
  AudioSourceStateLoading,
  AudioSourceStateReady
} from './constants.js'
import { createReadyStatusEvent, createErrorStatusEvent } from '../event/index.js'
import { EventName } from '../event/EventName.js'
import { EventTarget } from '../event/EventTarget.js'
import { promises } from 'fs'
import { AudioType } from './AudioType.js'

const { readFile } = promises

/**
 * An audio resource that can be rendered to a destination output buffer.
 *
 * @memberof module:@lse/core
 * @extends module:@lse/core.EventTarget
 * @hideconstructor
 */
class AudioSource extends EventTarget {
  _type = AudioType.NULL
  _uri = ''
  _state = AudioSourceStateInit
  _native = null
  _buffer = null

  constructor (native, type, uri) {
    super([EventName.status])

    this._native = native
    this._type = type
    this._uri = uri
    this._buffer = Buffer.isBuffer(uri) ? uri : null
  }

  /**
   * Get the destination buffer type this audio source renders to.
   *
   * @type {AudioType}
   */
  get type () {
    return this._type
  }

  /**
   * Get the filename or buffer used to load this audio source.
   *
   * @type {string|buffer}
   */
  get uri () {
    return this._uri
  }

  /**
   * Is the audio source in the ready state?
   *
   * In the ready state, playback can be initiated.
   *
   * @returns {boolean}
   */
  isReady () {
    return this._state === AudioSourceStateReady
  }

  /**
   * Is the audio source in the loading state?
   *
   * @returns {boolean}
   */
  isLoading () {
    return this._state === AudioSourceStateLoading
  }

  /**
   * Is the audio source in the error state?
   *
   * @returns {boolean}
   */
  isError () {
    return this._state === AudioSourceStateError
  }

  /**
   * Start playback of the source from the beginning.
   *
   * @param {Number} [opts.loops=1] The number of times to repeat playback. If 0, playback will loop forever.
   * @param {Number} [opts.fadeInMs=0] The time in milliseconds to fade in (volume) playback.
   */
  play (opts = {}) {
    this._native?.play(opts)
  }

  /**
   * Get the volume of this audio source.
   *
   * If hasVolume() returns false, 0 is returned.
   *
   * @returns volume {number} Range [0-1]
   */
  get volume () {
    return this._native?.getVolume()
  }

  /**
   * Sets the volume of this audio sources.
   *
   * If hasVolume() returns false, this method is a no-op
   *
   * @param value {number} A number between [0-1]. If the value is not a number, volume is set to 0. If the value is
   * out of range, it is Math.clamp()'d to [0-1].
   */
  set volume (value) {
    this._native?.setVolume(isNumber(value) ? clamp(value, 0, 1) : 0)
  }

  /**
   * Does this source support changing volume?
   *
   * If true, the volume property of AudioSource is honored.
   *
   * @returns {boolean}
   */
  hasVolume () {
    return this._hasCapability(AudioSourceCapabilityVolume)
  }

  /**
   * Does this support looping playback?
   *
   * If true, the loops argument of play is honored.
   *
   * @returns {boolean}
   */
  canLoop () {
    return this._hasCapability(AudioSourceCapabilityLoop)
  }

  /**
   * Does this support fading in on the start of playback?
   *
   * If true, the fadeInMs argument of play is honored.
   *
   * @returns {boolean}
   */
  canFadeIn () {
    return this._hasCapability(AudioSourceCapabilityFadeIn)
  }

  /**
   * @ignore
   */
  $load (sync, defer) {
    if (this._state !== AudioSourceStateInit) {
      throw Error('expected init state')
    }

    const { _native } = this

    if (!_native) {
      // No native source installed. Just error out. The audio type is probably not supported.
      this._state = AudioSourceStateError
      this.dispatchEvent(createErrorStatusEvent(this, { message: 'no native source' }), defer)
    } else if (this._buffer) {
      // A backing buffer exists, use that, regardless of sync
      this._loadAndDispatch(this._buffer, defer)
    } else if (sync) {
      // synchronously load the uri. this is a filename
      this._loadAndDispatch(this._uri, defer)
    } else {
      // asynchronously load the uri. set the state to loading in the meantime.
      this._state = AudioSourceStateLoading

      readFile(this._uri)
        .then(buffer => {
          if (this._state !== AudioSourceStateLoading) {
            // if no longer in loading state, $unload was called, cancelling this read request
            return
          }

          this._buffer = buffer
          this._loadAndDispatch(buffer, defer)
        })
        .catch(error => {
          if (this._state !== AudioSourceStateLoading) {
            // if no longer in loading state, $unload was called, cancelling this read request
            return
          }

          this._state = AudioSourceStateError
          this.dispatchEvent(createErrorStatusEvent(this, error), defer)
        })
    }
  }

  /**
   * @ignore
   */
  $unload () {
    this._state = AudioSourceStateInit
    this._native?.unload()
  }

  /**
   * @ignore
   */
  $destroy () {
    this._native.destroy()
    this._state = -1
    this._uri = this._buffer = this._native = null
  }

  /**
   * @ignore
   */
  _hasCapability (capability) {
    return this._native?.hasCapability(capability) ?? false
  }

  /**
   * @ignore
   */
  _loadAndDispatch (uri, defer) {
    try {
      this._native.load(uri)
    } catch (e) {
      this._state = AudioSourceStateError
      this.dispatchEvent(createErrorStatusEvent(this, e), defer)
      return
    }

    this._state = AudioSourceStateReady
    this.dispatchEvent(createReadyStatusEvent(this), defer)
  }
}

export { AudioSource }
