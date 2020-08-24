/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { $attach, $destroy, $detach } from '../util/InternalSymbols'
import { NullAudioSource, SampleAudioSource, StreamAudioSource } from './AudioSource'
import { AudioDestination } from './AudioDestination'
import { EventEmitter } from 'events'
import { emptyArray } from '../util'

const $stage = Symbol('stage')
const $sample = Symbol('sample')
const $stream = Symbol('stream')
const $emitter = Symbol('emitter')
const $nullAudioSource = Symbol('nullAudioSource')
const $add = Symbol('add')
const $audioSourceMap = Symbol('audioSourceMap')
const $unloadAudioSources = Symbol('unloadAudioSources')

const $plugin = Symbol.for('plugin')
const $load = Symbol.for('load')
const $unload = Symbol.for('unload')
const $setNativeDestination = Symbol.for('setNativeDestination')

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
    this[$emitter] = new EventEmitter()
    this[$nullAudioSource] = Object.freeze(new NullAudioSource(this))
  }

  // TODO: document events ('status')

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
   * Checks if the audio manager is attached to the platform audio subsystem. If not attached, audio playback will
   * not function.
   *
   * @returns true if attached to platform audio subsystem; otherwise, false
   */
  isAttached () {
    return this[$plugin] && this[$plugin].attached
  }

  /**
   * @returns {string[]} List of audio device names available on this system.
   */
  getDevices () {
    // Note: names are purely informational right now
    return this[$plugin] ? this[$plugin].devices : emptyArray
  }

  /**
   * @returns {AudioDestination} The sound effect output buffer.
   */
  get sample () {
    return this[$sample]
  }

  /**
   * @returns {AudioDestination} The background music output buffer.
   */
  get stream () {
    return this[$stream]
  }

  /**
   * Get an AudioSource by id.
   *
   * @param id {string} The id used when the AudioSource was added with addSample() or addStream().
   * @returns {AudioSource} AudioSource object matching the id; otherwise, a null-type AudioSource object is returned.
   */
  get (id) {
    return this[$audioSourceMap].get(id) || this[$nullAudioSource]
  }

  /**
   * Checks if an AudioSource has been added.
   *
   * @param id {string} AudioSource id
   * @returns {boolean} true if AudioSource with id exists; otherwise, false
   */
  has (id) {
    return this[$audioSourceMap].has(id)
  }

  /**
   * Delete an AudioSource by id.
   *
   * If no AudioSource exists with the given id, the call is a no-op.
   *
   * @param id {string} AudioSource id
   */
  delete (id) {
    this[$audioSourceMap].delete(id)
  }

  /**
   * Adds a new audio sample (sound effect).
   *
   * If id has already been added, this call is equivalent to get(id).
   *
   * The sync flag will load the audio file from disk immediately. If the audio plugin is not attached, the returned
   * AudioSource will not be in the ready state. When the audio plugin is attached, the AudioSource will finish
   * loading. Additionally, the status of the AudioSource object is updated immediately, but the status event will
   * be dispatched on the microtask queue.
   *
   * @param id {string} the path or file uri for an audio file
   * @param opts {Object} AudioSource loading options
   * @param {boolean} [opts.sync=false] if true, the AudioSource is loaded synchronously; otherwise, the AudioSource
   * is loaded asynchronously.
   * @returns {AudioSource}
   */
  addSample (id, opts = {}) {
    return this[$add](id, opts, SampleAudioSource)
  }

  /**
   * Adds a new audio stream (background music).
   *
   * If id has already been added, this call is equivalent to get(id).
   *
   * The sync flag will load the audio file from disk immediately. If the audio plugin is not attached, the returned
   * AudioSource will not be in the ready state. When the audio plugin is attached, the AudioSource will finish
   * loading. Additionally, the status of the AudioSource object is updated immediately, but the status event will
   * be dispatched on the microtask queue.
   *
   * @param id {string} the path or file uri for an audio file
   * @param opts {Object} AudioSource loading options
   * @param {boolean} [opts.sync=false] if true, the AudioSource is loaded synchronously; otherwise, the AudioSource
   * is loaded asynchronously.
   * @returns {AudioSource}
   */
  addStream (id, opts = {}) {
    return this[$add](id, opts, StreamAudioSource)
  }

  /**
   * @returns {AudioSource[]} Array of AudioSource objects that have been added to the AudioManager
   */
  all () {
    return Array.from(this[$audioSourceMap].values())
  }

  /**
   * @ignore
   */
  [$attach] () {
    if (!this[$plugin] || this[$plugin].attached) {
      return
    }

    this[$plugin].attach()
    this[$sample][$setNativeDestination](this[$plugin].createSampleAudioDestination())
    this[$stream][$setNativeDestination](this[$plugin].createStreamAudioDestination())
    this[$audioSourceMap].forEach(as => as[$load]())
    this[$emitter].emit('attached')
  }

  /**
   * @ignore
   */
  [$detach] () {
    if (!this[$plugin] || !this[$plugin].attached) {
      return
    }

    this[$unloadAudioSources]()
    this[$sample][$setNativeDestination](null)
    this[$stream][$setNativeDestination](null)
    this[$plugin].detach()
    this[$emitter].emit('detached')
  }

  /**
   * @ignore
   */
  [$destroy] () {
    if (this[$plugin]) {
      this[$unloadAudioSources]()
      this[$audioSourceMap].clear()
      this[$sample][$setNativeDestination](null)
      this[$stream][$setNativeDestination](null)
      this[$plugin] = null
    }
  }

  /**
   * @ignore
   */
  [$add] (id, opts, AudioSourceClass) {
    if (this.has(id)) {
      return this.get(id)
    }

    const as = new AudioSourceClass(this, id)

    this[$audioSourceMap].set(id, as)

    as[$load](!!opts.sync)

    return as
  }

  /**
   * @ignore
   */
  [$unloadAudioSources] () {
    this[$audioSourceMap].forEach(as => as[$unload]())
  }
}
