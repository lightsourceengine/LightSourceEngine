/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { $destination } from '../util/InternalSymbols'
import { AudioDecoder } from './AudioDecoder'
import {
  AudioDestinationCapabilityFadeOut,
  AudioDestinationCapabilityPause,
  AudioDestinationCapabilityResume,
  AudioDestinationCapabilityStop,
  AudioDestinationCapabilityVolume
} from './constants'
import { clamp, isNumber, emptyArray } from '../util'

const $decoders = Symbol('decoders')
const $rawDecoders = Symbol('rawDecoders')
const $setNativeDestination = Symbol.for('setNativeDestination')

/**
 * An audio output buffer.
 */
export class AudioDestination {
  constructor () {
    this[$destination] = NullNativeAudioDestination.instance
    this[$decoders] = emptyArray
    this[$rawDecoders] = emptyArray
  }

  /**
   * @returns {boolean} true if audio sources can be rendered to this destination; false if no audio can
   * be loaded or played with this destination.
   */
  isAvailable () {
    return this[$destination] !== NullNativeAudioDestination.instance
  }

  /**
   * @returns {string[]} List of audio decoders this destination supports.
   */
  getDecoders () {
    return this[$decoders]
  }

  /**
   * @ignore
   */
  getRawDecoders () {
    return this[$rawDecoders]
  }

  /**
   * Checks if an audio decoder, defined in AudioDecoder, is supported.
   *
   * @param decoder {string} Name of decoder to check. Should be a name from AudioDecoder.
   * @returns {boolean} true if decoder is supported; otherwise, false
   */
  hasDecoder (decoder) {
    return this[$rawDecoders].indexOf(decoder) >= 0
  }

  /**
   * @returns {boolean} true if the audio destination can be pause()'d; otherwise, false
   */
  canPause () {
    return this[$destination].hasCapability(AudioDestinationCapabilityPause)
  }

  /**
   * @returns {boolean} true if the audio destination can be resume()'d; otherwise, false
   */
  canResume () {
    return this[$destination].hasCapability(AudioDestinationCapabilityResume)
  }

  /**
   * @returns {boolean} true if the audio destination can be stop()'d; otherwise, false
   */
  canStop () {
    return this[$destination].hasCapability(AudioDestinationCapabilityStop)
  }

  /**
   * @returns {boolean} true if the audio destination can be faded out; otherwise, false
   */
  canFadeOut () {
    return this[$destination].hasCapability(AudioDestinationCapabilityFadeOut)
  }

  /**
   * @returns {boolean} true if the audio destination has volume controls; otherwise, false
   */
  hasVolume () {
    return this[$destination].hasCapability(AudioDestinationCapabilityVolume)
  }

  /**
   * @returns volume {number} Current value [0-1] of the volume of the audio destination.
   */
  getVolume () {
    return this[$destination].volume
  }

  /**
   * Sets the volume.
   *
   * @param value {number} A value between [0-1]. If the value is not a number, volume is set to 0. If the value is
   * out of range, it is Math.clamp()'d to [0-1].
   */
  setVolume (value) {
    this[$destination].volume = isNumber(value) ? clamp(value, 0, 1) : 0
  }

  /**
   * Pause playback of the output buffer.
   */
  pause () {
    this[$destination].pause()
  }

  /**
   * Resume playback of the output buffer.
   */
  resume () {
    this[$destination].resume()
  }

  /**
   * Stop playback of the audio buffer.
   *
   * @param {Number} [opts.fadeOutMs] Time in milliseconds to fade out the audio before stopping. Only available if
   * canFadeOut is true.
   */
  stop (opts) {
    this[$destination].stop(opts)
  }

  /**
   * @ignore
   */
  [$setNativeDestination] (nativeDestination) {
    this[$destination] = nativeDestination || NullNativeAudioDestination.instance
    this[$rawDecoders] = [...this[$destination].decoders]
    this[$decoders] = []

    for (const rawDecoder of this[$rawDecoders]) {
      if (rawDecoder in AudioDecoder) {
        this[$decoders].push(rawDecoder)
      }
    }

    Object.freeze(this[$rawDecoders])
    Object.freeze(this[$decoders])
  }
}

/**
 * @ignore
 */
class NullNativeAudioDestination {
  static instance = new NullNativeAudioDestination()
  constructor () { this.volume = 0; this.decoders = emptyArray }
  resume () {}
  stop () {}
  pause () {}
  hasCapability () { return false }
}
