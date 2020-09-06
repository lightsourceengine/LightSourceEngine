/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { NullAudioSource, SampleAudioSource, StreamAudioSource } from './AudioSource.js'
import { AudioDestination } from './AudioDestination.js'
import { emptyArray, EventEmitter } from '../util/index.js'
import { AttachedEvent, DetachedEvent, EventNames } from '../event/index.js'

/**
 * Audio API.
 */
export class AudioManager {
  _stage = null
  _plugin = null
  _audioSourceMap = new Map()
  _sampleAudioDestination = new AudioDestination()
  _streamAudioDestination = new AudioDestination()
  _emitter = new EventEmitter([EventNames.attached, EventNames.detached])
  _nullAudioSource = Object.freeze(new NullAudioSource(this, ''))

  constructor (stage) {
    this._stage = stage
  }

  // TODO: document events ('status')

  on (name, callback) {
    this._emitter.on(name, callback)
  }

  once (name, callback) {
    this._emitter.once(name, callback)
  }

  off (name, callback) {
    this._emitter.off(name, callback)
  }

  isAvailable () {
    return !!this._plugin
  }

  /**
   * Checks if the audio manager is attached to the platform audio subsystem. If not attached, audio playback will
   * not function.
   *
   * @returns true if attached to platform audio subsystem; otherwise, false
   */
  isAttached () {
    return !!this._plugin?.attached
  }

  /**
   * @returns {string[]} List of audio device names available on this system.
   */
  getDevices () {
    // Note: names are purely informational right now
    return this._plugin?.devices ?? emptyArray
  }

  /**
   * @returns {AudioDestination} The sound effect output buffer.
   */
  get sample () {
    return this._sampleAudioDestination
  }

  /**
   * @returns {AudioDestination} The background music output buffer.
   */
  get stream () {
    return this._streamAudioDestination
  }

  /**
   * Get an AudioSource by id.
   *
   * @param id {string} The id used when the AudioSource was added with addSample() or addStream().
   * @returns {AudioSource} AudioSource object matching the id; otherwise, a null-type AudioSource object is returned.
   */
  get (id) {
    return this._audioSourceMap.get(id) || this._nullAudioSource
  }

  /**
   * Checks if an AudioSource has been added.
   *
   * @param id {string} AudioSource id
   * @returns {boolean} true if AudioSource with id exists; otherwise, false
   */
  has (id) {
    return this._audioSourceMap.has(id)
  }

  /**
   * Delete an AudioSource by id.
   *
   * If no AudioSource exists with the given id, the call is a no-op.
   *
   * @param id {string} AudioSource id
   */
  delete (id) {
    const audioSource = this._audioSourceMap.get(id)

    if (audioSource) {
      audioSource.$unload()
      this._audioSourceMap.delete(id)
    }
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
    return this._add(id, opts, SampleAudioSource)
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
    return this._add(id, opts, StreamAudioSource)
  }

  /**
   * @returns {AudioSource[]} Array of AudioSource objects that have been added to the AudioManager
   */
  all () {
    return Array.from(this._audioSourceMap.values())
  }

  /**
   * @ignore
   */
  $attach () {
    if (!this._plugin || this._plugin.attached) {
      return
    }

    this._plugin.attach()
    this._sampleAudioDestination.$setNative(this._plugin.createSampleAudioDestination())
    this._streamAudioDestination.$setNative(this._plugin.createStreamAudioDestination())
    this._audioSourceMap.forEach(as => as.$load())
    this._emitter.emitEvent(AttachedEvent(this))
  }

  /**
   * @ignore
   */
  $detach () {
    if (!this._plugin || !this._plugin.attached) {
      return
    }

    this._unloadAudioSources()
    this._sampleAudioDestination.$setNative(null)
    this._streamAudioDestination.$setNative(null)
    this._plugin.detach()
    this._emitter.emitEvent(DetachedEvent(this))
  }

  /**
   * @ignore
   */
  $destroy () {
    if (this._plugin) {
      this._unloadAudioSources()
      this._audioSourceMap.clear()
      this._sampleAudioDestination.$setNative(null)
      this._streamAudioDestination.$setNative(null)
      this._plugin.destroy()
      this._plugin = null
    }
  }

  /**
   * @ignore
   */
  $setPlugin (instance) {
    this._plugin = instance
  }

  /**
   * @ignore
   */
  _add (id, opts, AudioSourceClass) {
    if (this.has(id)) {
      return this.get(id)
    }

    const as = new AudioSourceClass(this, id)

    this._audioSourceMap.set(id, as)

    as.$load(!!opts.sync)

    return as
  }

  /**
   * @ignore
   */
  _unloadAudioSources () {
    this._audioSourceMap.forEach(as => as.$unload())
  }
}
