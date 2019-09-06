/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { $destination } from '../util/InternalSymbols'
import { normalizeVolume } from './normalizeVolume'
import {
  AudioDestinationCapabilityFadeOut,
  AudioDestinationCapabilityPause,
  AudioDestinationCapabilityResume,
  AudioDestinationCapabilityStop,
  AudioDestinationCapabilityVolume
} from './constants'

/**
 * An audio output buffer.
 */
export class AudioDestination {
  constructor () {
    this[$destination] = null
  }

  /**
   * Is this destination available on this system?
   *
   * When a destination is unavailable, all other APIs will have undefined behavior.
   *
   * @returns {boolean} if native support is available for this destination
   */
  get available () {
    return !!this[$destination]
  }

  /**
   * List of decoders or AudioSource formats this destination supports.
   *
   * @returns {string[]}
   */
  get decoders () {
    // TODO: the decoders are in SDL Mixer format. Need to translate them to a consistent enum.
    return this[$destination].decoders
  }

  /**
   * Is the pause() API available?
   *
   * @returns {boolean}
   */
  get canPause () {
    return this[$destination].hasCapability(AudioDestinationCapabilityPause)
  }

  /**
   * Is the resume() API available?
   *
   * @returns {boolean}
   */
  get canResume () {
    return this[$destination].hasCapability(AudioDestinationCapabilityResume)
  }

  /**
   * Is the stop() API available?
   *
   * @returns {boolean}
   */
  get canStop () {
    return this[$destination].hasCapability(AudioDestinationCapabilityStop)
  }

  /**
   * Is the stop({fadeOutMs: 100}) API available?
   *
   * @returns {boolean}
   */
  get canFadeOut () {
    return this[$destination].hasCapability(AudioDestinationCapabilityFadeOut)
  }

  /**
   * Is the volume API available?
   *
   * @returns {boolean}
   */
  get hasVolume () {
    return this[$destination].hasCapability(AudioDestinationCapabilityVolume)
  }

  /**
   * Control the volume of the output buffer.
   *
   * Volume must be a floating point number between [0-1].
   *
   * @property volume
   */

  get volume () {
    return this[$destination].volume
  }

  set volume (value) {
    this[$destination].volume = normalizeVolume(value)
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
}
