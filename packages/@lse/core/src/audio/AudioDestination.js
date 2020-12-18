/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { AudioDecoderType } from './AudioDecoderType.js'
import {
  AudioDestinationCapabilityFadeOut,
  AudioDestinationCapabilityPause,
  AudioDestinationCapabilityResume,
  AudioDestinationCapabilityStop,
  AudioDestinationCapabilityVolume
} from './constants.js'
import { clamp, isNumber, emptyArray } from '../util/index.js'

/**
 * An audio output buffer.
 */
export class AudioDestination {
  _native = NullNativeAudioDestination.instance // eslint-disable-line
  _decoders = emptyArray
  _rawDecoders = emptyArray

  /**
   * @returns {boolean} true if audio sources can be rendered to this destination; false if no audio can
   * be loaded or played with this destination.
   */
  isAvailable () {
    return this._native !== NullNativeAudioDestination.instance
  }

  /**
   * @returns {string[]} List of audio decoders this destination supports.
   */
  getDecoders () {
    return this._decoders
  }

  /**
   * @ignore
   */
  getRawDecoders () {
    return this._rawDecoders
  }

  /**
   * Checks if an audio decoder, defined in AudioDecoder, is supported.
   *
   * @param decoder {string} Name of decoder to check. Should be a name from AudioDecoder.
   * @returns {boolean} true if decoder is supported; otherwise, false
   */
  hasDecoder (decoder) {
    return this._rawDecoders.indexOf(decoder) >= 0
  }

  /**
   * @returns {boolean} true if the audio destination can be pause()'d; otherwise, false
   */
  canPause () {
    return this._native.hasCapability(AudioDestinationCapabilityPause)
  }

  /**
   * @returns {boolean} true if the audio destination can be resume()'d; otherwise, false
   */
  canResume () {
    return this._native.hasCapability(AudioDestinationCapabilityResume)
  }

  /**
   * @returns {boolean} true if the audio destination can be stop()'d; otherwise, false
   */
  canStop () {
    return this._native.hasCapability(AudioDestinationCapabilityStop)
  }

  /**
   * @returns {boolean} true if the audio destination can be faded out; otherwise, false
   */
  canFadeOut () {
    return this._native.hasCapability(AudioDestinationCapabilityFadeOut)
  }

  /**
   * @returns {boolean} true if the audio destination has volume controls; otherwise, false
   */
  hasVolume () {
    return this._native.hasCapability(AudioDestinationCapabilityVolume)
  }

  /**
   * @returns volume {number} Current value [0-1] of the volume of the audio destination.
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
   * Pause playback of the output buffer.
   */
  pause () {
    this._native.pause()
  }

  /**
   * Resume playback of the output buffer.
   */
  resume () {
    this._native.resume()
  }

  /**
   * Stop playback of the audio buffer.
   *
   * @param {Number} [opts.fadeOutMs] Time in milliseconds to fade out the audio before stopping. Only available if
   * canFadeOut is true.
   */
  stop (opts) {
    this._native.stop(opts)
  }

  /**
   * @ignore
   */
  $createNativeAudioSource () {
    return this._native.createAudioSource()
  }

  /**
   * @ignore
   */
  $setNative (nativeDestination) {
    this._native = nativeDestination || NullNativeAudioDestination.instance
    this._decoders = []

    for (const rawDecoder of this._native.decoders) {
      if (rawDecoder in AudioDecoderType) {
        this._decoders.push(rawDecoder)
      }
    }

    this._rawDecoders = [...this._native.decoders]

    Object.freeze(this._rawDecoders)
    Object.freeze(this._decoders)
  }
}

/**
 * @ignore
 */
class NullNativeAudioDestination {
  static instance = new NullNativeAudioDestination()
  decoders = emptyArray
  resume () {}
  setVolume (value) {}
  getVolume () { return 0 }
  stop () {}
  pause () {}
  hasCapability () { return false }
}
