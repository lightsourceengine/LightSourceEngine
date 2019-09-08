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
  $setResourcePath,
  $adapter,
  $audioSourceMap
} from '../util/InternalSymbols'
import bindings from 'bindings'
import { SDLMixerModuleId, SDLModuleId } from '../addon'
import { logexcept } from '../util'
import { AudioSource, AudioSource$load, AudioSource$reset } from './AudioSource'
import { AudioDestination } from './AudioDestination'
import { AudioSourceTypeStream, AudioSourceTypeSample } from './constants'

const $stage = Symbol('stage')
const $resourcePath = Symbol('resourcePath')
const $sample = Symbol('sample')
const $stream = Symbol('stream')

/**
 * Audio API.
 */
export class AudioManager {
  constructor (stage) {
    this[$stage] = stage
    this[$adapter] = null
    this[$audioSourceMap] = new Map()
    this[$resourcePath] = stage.resourcePath
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
    return this[$adapter] ? this[$adapter].devices : []
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
    if (!this[$adapter].attached) {
      this[$adapter].attach()
      for (const audioSource of this[$audioSourceMap].values()) {
        AudioSource$load(audioSource, false)
      }
    }
  }

  /**
   * @ignore
   */
  [$detach] () {
    if (this[$adapter].attached) {
      for (const audioSource of this[$audioSourceMap].values()) {
        AudioSource$reset(audioSource)
      }
      logexcept(() => this[$adapter].detach(), 'AudioAdapter detach error: ')
    }
  }

  /**
   * @ignore
   */
  [$init] (audioAdapter) {
    if (this[$adapter]) {
      throw Error('AudioAdapter has already been initialized.')
    }

    this[$adapter] = createAudioAdapter(validateAudioAdapterConfig(audioAdapter))
    this[$sample][$destination] = this[$adapter].createSampleAudioDestination()
    this[$stream][$destination] = this[$adapter].createStreamAudioDestination()
  }

  /**
   * @ignore
   */
  [$destroy] () {
    if (this[$adapter]) {
      this[$detach]()
      logexcept(() => this[$adapter].detach(), 'AudioAdapter destroy error: ')
      this[$audioSourceMap].clear()
      this[$sample][$destination] = this[$stream][$destination] = null
      this[$adapter] = null
    }
  }

  /**
   * @ignore
   */
  [$setResourcePath] (value) {
    this[$resourcePath] = value
  }
}

const validateAudioAdapterConfig = (audioAdapter) => {
  if (typeof audioAdapter === 'function' || typeof audioAdapter === 'string') {
    return [audioAdapter]
  } else if (!audioAdapter) {
    return [SDLMixerModuleId, SDLModuleId]
  }

  throw Error('audioAdapter must be a module name string or an AudioAdapter class.')
}

const createAudioAdapterInternal = (adapter) => {
  let AudioAdapter = null

  if (typeof adapter === 'string') {
    try {
      AudioAdapter = bindings(adapter).AudioAdapter
    } catch (e) {
      console.log(`Module ${adapter} failed to load. Error: ${e.message}`)
      return null
    }

    if (typeof AudioAdapter !== 'function') {
      console.log(`Module ${adapter} does not contain an AudioAdapter class.`)
      return null
    }
  } else {
    AudioAdapter = adapter
  }

  let adapterInstance

  try {
    adapterInstance = new AudioAdapter()
  } catch (e) {
    console.log(`Failed to construct the AudioAdapter instance. Error: ${e.message}`)
    return null
  }

  try {
    adapterInstance.attach()
  } catch (e) {
    logexcept(() => adapterInstance.destroy())
    console.log(`Failed to initialize the AudioAdapter instance. Error: ${e.message}`)
    return null
  }

  return adapterInstance
}

const createAudioAdapter = (adapters) => {
  for (const adapter of adapters) {
    const audioAdapterInstance = createAudioAdapterInternal(adapter)

    if (audioAdapterInstance) {
      return audioAdapterInstance
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
