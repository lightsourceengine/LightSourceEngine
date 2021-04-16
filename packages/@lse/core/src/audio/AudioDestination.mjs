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

import { AudioDecoderType } from './AudioDecoderType.mjs'
import {
  AudioDestinationCapabilityFadeOut,
  AudioDestinationCapabilityPause,
  AudioDestinationCapabilityResume,
  AudioDestinationCapabilityStop,
  AudioDestinationCapabilityVolume
} from './constants.mjs'
import { clamp, isNumber, emptyArray } from '../util/index.mjs'
import { AudioType } from './AudioType.mjs'
import { AudioSource } from './AudioSource.mjs'

const nullAudioDestination = {
  getDecoders () { return emptyArray },
  destroy () {},
  resume () {},
  setVolume (value) {},
  getVolume () { return 0 },
  stop () {},
  pause () {},
  hasCapability () { return false }
}

/**
 * An audio output buffer.
 *
 * @memberof module:@lse/core
 * @hideconstructor
 */
class AudioDestination {
  _type = AudioType.NULL
  _native = nullAudioDestination
  _decoders = emptyArray
  _rawDecoders = emptyArray
  _sources = new Map()
  _isAttached = false

  constructor (type) {
    this._type = type
  }

  /**
   * Is this destination buffer available?
   *
   * Availability for a destination buffer means that the system or app configuration does not support
   * this type of audio destination buffer.
   *
   * @returns {boolean}
   */
  isAvailable () {
    return this._native && this._native !== nullAudioDestination
  }

  /**
   * List of audio decoders this destination supports.
   */
  get decoders () {
    return this._decoders
  }

  /**
   * Get the audio destination buffer type.
   *
   * @type {AudioType}
   */
  get type () {
    return this._type
  }

  /**
   * Checks if an audio decoder is supported.
   *
   * @param decoder {string} Case-insensitive decoder name. Known decoder names can be found in AudioDecoder.
   * @returns {boolean} true if decoder is supported; otherwise, false
   */
  hasDecoder (decoder) {
    return this._decoders.includes(decoder?.toUpperCase())
  }

  /**
   * Does this audio destination buffer support pausing of playback?
   *
   * @returns {boolean}
   */
  canPause () {
    return this._native.hasCapability(AudioDestinationCapabilityPause)
  }

  /**
   * Does this audio destination buffer support resuming of paused playback?
   *
   * @returns {boolean}
   */
  canResume () {
    return this._native.hasCapability(AudioDestinationCapabilityResume)
  }

  /**
   * Does this audio destination buffer support stopping of playback?
   *
   * @returns {boolean}
   */
  canStop () {
    return this._native.hasCapability(AudioDestinationCapabilityStop)
  }

  /**
   * Does this audio destination buffer support fading out of playback on stop?
   *
   * @returns {boolean}
   */
  canFadeOut () {
    return this._native.hasCapability(AudioDestinationCapabilityFadeOut)
  }

  /**
   * Does this audio destination buffer support volume controls?
   *
   * @returns {boolean}
   */
  hasVolume () {
    return this._native.hasCapability(AudioDestinationCapabilityVolume)
  }

  /**
   * Get the volume of this audio destination buffer.
   *
   * If hasVolume() returns false, 0 is returned.
   *
   * @returns volume {number} Range [0-1]
   */
  get volume () {
    return this._native.getVolume()
  }

  /**
   * Sets the volume of this audio destination buffer.
   *
   * If hasVolume() returns false, this method is a no-op.
   *
   * @param value {number} A number between [0-1]. If the value is not a number, volume is set to 0. If the value is
   * out of range, it is Math.clamp()'d to [0-1].
   */
  set volume (value) {
    this._native.setVolume(isNumber(value) ? clamp(value, 0, 1) : 0)
  }

  /**
   * Pause playback of this buffer.
   *
   * If canPause() returns false, this method is a no-op.
   */
  pause () {
    this._native.pause()
  }

  /**
   * Resume playback of this buffer.
   *
   * If canResume() returns false, this method is a no-op.
   */
  resume () {
    this._native.resume()
  }

  /**
   * Stop playback of this buffer.
   *
   * If canStop() returns false, this method is a no-op.
   *
   * @param {Number} [opts.fadeOutMs] Time in milliseconds to fade out the audio before stopping.
   */
  stop (opts) {
    this._native.stop(opts)
  }

  /**
   * List all cached audio sources owned by this audio buffer.
   *
   * The returned list is not stable (order and position).
   *
   * @returns {AudioSource[]}
   */
  all () {
    return Array.from(this._sources.values())
  }

  /**
   * Load and cache an audio source.
   *
   * @param {string} options Filename
   * @param {Object} options
   * @param {string|Buffer} [options.uri] The audio file to load
   * @param {string} [options.key] Cache key. If not set, the uri parameter will be used. If uri is a buffer, this
   * parameter must be provided
   * @param {bool} [options.sync] Flag to load audio source synchronously
   *
   * @returns {AudioSource}
   * @throws {Error} if options arg is malformed
   */
  add (options) {
    if (typeof options === 'string') {
      options = { uri: options }
    }

    let { key, uri, sync } = options

    if (Buffer.isBuffer(uri)) {
      if (!key || typeof key !== 'string') {
        throw Error('key is required')
      }
    } else if (typeof uri === 'string') {
      if (!key || typeof key !== 'string') {
        key = uri
      }
    } else {
      throw Error('uri must be a filename or Buffer')
    }

    if (this.has(key)) {
      throw Error(`source key '${key}' already exists`)
    }

    const source = this._createAudioSource(uri)

    this._sources.set(key, source)

    if (this._isAttached) {
      source.$load(sync, true)
    }

    return source
  }

  /**
   * Checks if an audio source is cached by cache key.
   *
   * @param key {string} Cache key to search on
   * @returns {boolean}
   */
  has (key) {
    return this._sources.has(key)
  }

  /**
   * Gets a cached audio source by cache key.
   *
   * @param key {string} Cache key to search on
   * @returns {boolean}
   */
  get (key) {
    return this._sources.get(key)
  }

  /**
   * Remove an audio source by cache key.
   *
   * @param key {string} Cache key to search on
   */
  delete (key) {
    const { _sources } = this

    _sources.get(key)?.$destroy()
    _sources.delete(key)
  }

  /**
   * Shorthand function for adding a source to the cache synchronously playing it. If the source is cached,
   * it will be used for playback.
   *
   * @param {string} options filename of cache key
   * @param {object} options
   * @param {string|Buffer} [options.uri]
   * @param {string} [options.key] cache key
   * @param {Number} [options.loops] The number of times to repeat playback. If 0, playback will loop forever.
   * @param {Number} [options.fadeInMs] The time in milliseconds to fade in (volume) playback.
   * @returns {AudioSource}
   */
  play (options) {
    let source

    if (typeof options === 'string') {
      source = this.get(options)

      if (!source) {
        source = this.add({ uri: options, sync: true })
      }

      source.play()
    } else {
      source = this.get(options.key ?? options.uri)

      if (!source) {
        source = this.add({ ...options, sync: true })
      }

      source.play(options)
    }

    return source
  }

  /**
   * @ignore
   */
  get rawDecoders () {
    return this._rawDecoders
  }

  /**
   * @ignore
   */
  $setPlugin (plugin) {
    let native = null

    switch (this._type) {
      case AudioType.SAMPLE:
        native = plugin.createSampleAudioDestination()
        break
      case AudioType.STREAM:
        native = plugin.createStreamAudioDestination()
        break
    }

    this._native = native ?? nullAudioDestination

    const nativeDecoders = this._native.getDecoders()
    const knownDecoders = nativeDecoders
      .map(decoder => decoder.toUpperCase())
      .filter(decoder => decoder in AudioDecoderType)

    this._decoders = Object.freeze(knownDecoders)
    this._rawDecoders = Object.freeze(nativeDecoders)
  }

  /**
   * @ignore
   */
  $attach () {
    const { _isAttached, _sources } = this

    if (_isAttached) {
      return
    }

    _sources.forEach(source => source.$load(true, false))

    this._isAttached = true
  }

  /**
   * @ignore
   */
  $detach () {
    const { _isAttached, _sources } = this

    if (!_isAttached) {
      return
    }

    _sources.forEach(source => source.$unload())

    this._isAttached = false
  }

  /**
   * @ignore
   */
  $destroy () {
    const { _native, _sources } = this

    if (_native) {
      _sources.forEach(source => source.$destroy())
      _sources.clear()
      _native.destroy()

      this._native = null
    }
  }

  /**
   * @ignore
   */
  _createAudioSource (uri) {
    return new AudioSource(this._native.createAudioSource(), this._type, uri)
  }
}

export { AudioDestination }
