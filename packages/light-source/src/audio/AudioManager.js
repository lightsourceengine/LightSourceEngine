/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import {
  $attach,
  $destination,
  $destroy,
  $detach,
  $init,
  $plugin,
  $audioSourceMap
} from '../util/InternalSymbols'
import { logexcept } from '../util'
import { AudioSource, AudioSource$load, AudioSource$reset } from './AudioSource'
import { AudioDestination } from './AudioDestination'
import { AudioSourceTypeStream, AudioSourceTypeSample } from './constants'

const $stage = Symbol('stage')
const $sample = Symbol('sample')
const $stream = Symbol('stream')

/**
 * Audio API.
 */
export class AudioManager {
  constructor (stage) {
    this[$stage] = stage
    this[$plugin] = null
    this[$audioSourceMap] = new Map()
    this[$sample] = new AudioDestination()
    this[$stream] = new AudioDestination()
  }

  /**
   * Get the names of audio devices available on this system.
   *
   * @returns {string[]}
   */
  get devices () {
    // Note: names are purely informational right now
    return this[$plugin] ? this[$plugin].devices : []
  }

  /**
   * Sound effect output buffer API.
   *
   * @returns {AudioDestination}
   */
  get sample () {
    return this[$sample]
  }

  /**
   * Background music output buffer API.
   *
   * @returns {AudioDestination}
   */
  get stream () {
    return this[$stream]
  }

  /**
   * Get an audio resource.
   *
   * @param id Resource ID.
   * @returns {AudioSource} if a resource does not exist for the given ID, null is returned.
   */
  getAudioSource (id) {
    return this[$audioSourceMap].get(id) || null
  }

  /**
   * Add a new sound effect or background music file as an AudioResource.
   *
   * Resource loading is performed asynchronously (unless options.sync is set). The resource is returned in a
   * 'loading' state while file IO and audio decoding are performed in the background. When the background work
   * finishes, the resource is placed in the 'ready' state and is then playable. If an error occurred asynchronously,
   * the resource will be put into the 'error' state.
   *
   * @param {Object|string} options Loading options. If string, the options are assumed to be a URI.
   * @param {string} options.uri The audio file to load. URI can be a file name (local or absolute) or a file URI with
   * a resource host name (file://resource/path_to_audio_file.wav).
   * @param {string} [options.id=null] Unique resource ID. If not set, the URI will be used as the resource ID.
   * @param {boolean} [options.sync=false] If true, the resource will loaded synchronously.
   * @param {('sample'|'stream')} [options.type='sample'] The type describes how the audio file will be decoded and
   * rendered to sound hardware. 'stream' is for background music that will be decoded as needed and play continuously.
   * 'sample' is for sound effects that are completely decoded into memory and usually played once. If selected type
   * is unavailable (see this.stream and this.type), the resource will fail to load.
   * @throws Error if options are malformed or an AudioResource already exists with the same ID
   * @returns {AudioSource}
   */
  addAudioSource (options) {
    options = parseAudioSourceOpts(options)

    const { id } = options
    const audioSourceMap = this[$audioSourceMap]

    if (audioSourceMap.has(id)) {
      throw Error(`AudioSource with id=${id} already exists.`)
    }

    const audioSource = new AudioSource(this, options)

    audioSourceMap.set(id, audioSource)

    AudioSource$load(audioSource, true)

    return audioSource
  }

  /**
   * Get a List of AudioSource objects added to the AudioManager.
   *
   * @returns {AudioSource[]}
   */
  all () {
    return Array.from(this[$audioSourceMap].values())
  }

  /**
   * @ignore
   */
  [$attach] () {
    if (!this[$plugin].attached) {
      this[$plugin].attach()
      for (const audioSource of this[$audioSourceMap].values()) {
        AudioSource$load(audioSource, false)
      }
    }
  }

  /**
   * @ignore
   */
  [$detach] () {
    if (this[$plugin].attached) {
      for (const audioSource of this[$audioSourceMap].values()) {
        AudioSource$reset(audioSource)
      }
      logexcept(() => this[$plugin].detach(), 'AudioAdapter detach error: ')
    }
  }

  /**
   * @ignore
   */
  [$init] (audioPluginInstance) {
    if (this[$plugin]) {
      throw Error('AudioAdapter has already been initialized.')
    }

    this[$plugin] = audioPluginInstance
    this[$sample][$destination] = audioPluginInstance.createSampleAudioDestination()
    this[$stream][$destination] = audioPluginInstance.createStreamAudioDestination()
  }

  /**
   * @ignore
   */
  [$destroy] () {
    if (this[$plugin]) {
      this[$detach]()
      this[$audioSourceMap].clear()
      this[$sample][$destination] = this[$stream][$destination] = null
      this[$plugin] = null
    }
  }
}

const parseAudioSourceOpts = (opts) => {
  // Make a copy of opts and normalize to an object.
  if (opts && typeof opts === 'string') {
    opts = { uri: opts }
  } else if (typeof opts === 'object') {
    opts = { ...opts }
  } else {
    throw Error('AudioSource options must be a string or object.')
  }

  // uri must be a string
  if (!opts.uri || typeof opts.uri !== 'string') {
    throw Error(`Invalid AudioSource uri: ${opts.uri}`)
  }

  // If id is not explicitly set, use uri.
  if (!opts.id) {
    opts.id = opts.uri
  }

  if (!opts.type) {
    // if type not set, default value is sample
    opts.type = AudioSourceTypeSample
  } else if (opts.type !== AudioSourceTypeStream && opts.type !== AudioSourceTypeSample) {
    throw Error(`Invalid AudioSource type: ${opts.type}`)
  }

  return opts
}
