/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { emptyArray } from '../util/index.js'
import { createAttachedEvent, createDetachedEvent } from '../event/index.js'
import { EventName } from '../event/EventName.js'
import { EventTarget } from '../event/EventTarget.js'
import { AudioDestination } from './AudioDestination.js'
import { AudioType } from './AudioType.js'

/**
 * Audio API.
 */
export class AudioManager extends EventTarget {
  _plugin = null
  _sample = new AudioDestination(AudioType.SAMPLE)
  _stream = new AudioDestination(AudioType.STREAM)
  _isAttached = false
  _deviceNames = emptyArray

  constructor () {
    super([EventName.onAttached, EventName.onDetached])
  }

  /**
   * Is the audio manager available?
   *
   * Availability for the audio manager means that the system's native audio APIs are not accessible. Either
   * the stage has not been configured or the system does not have a viable native audio API plugin.
   *
   * @returns {boolean}
   */
  isAvailable () {
    return !!this._plugin
  }

  /**
   * Is the audio manager attached?
   *
   * If attached, the audio manager is connected to the system's native audio APIs.
   *
   * @returns {boolean}
   */
  isAttached () {
    return this._isAttached
  }

  /**
   * List of audio device names available on the system.
   *
   * If the audio plugin is configured or not available on the system, this will return an empty array.
   *
   * @returns {string[]}
   */
  get devices () {
    // Note: names are purely informational right now
    return this._deviceNames
  }

  /**
   * Get the sample audio destination buffer.
   *
   * Multiple audio play requests can be rendered to the sample buffer at the same time, so the sample
   * buffer is good for sound effects.
   *
   * If the audio plugin is configured or not available on the system, the sample destination buffer will not
   * be available, but this variable and APIs are safe to call.
   *
   * @returns {AudioDestination}
   */
  get sample () {
    return this._sample
  }

  /**
   * Ge the stream audio destination buffer.
   *
   * A single audio play request can be rendered to the stream buffer. The stream buffer is intended for playing
   * background music or an audio track.
   *
   * If the audio plugin is configured or not available on the system, the stream destination buffer will not
   * be available, but this variable and APIs are safe to call.
   *
   * @returns {AudioDestination}
   */
  get stream () {
    return this._stream
  }

  /**
   * @ignore
   */
  $attach () {
    const { _plugin, _isAttached, _sample, _stream } = this

    if (!_plugin || _isAttached) {
      return
    }

    _sample.$attach()
    _stream.$attach()

    this._isAttached = true
    this.dispatchEvent(createAttachedEvent(this))
  }

  /**
   * @ignore
   */
  $setPlugin (plugin) {
    const { _sample, _stream } = this

    this._deviceNames = Object.freeze(plugin?.getDevices() ?? [])
    _sample.$setPlugin(plugin)
    _stream.$setPlugin(plugin)

    this._plugin = plugin
  }

  /**
   * @ignore
   */
  $detach () {
    const { _plugin, _isAttached, _sample, _stream } = this

    if (!_plugin || !_isAttached) {
      return
    }

    _stream.$detach()
    _sample.$detach()

    this._isAttached = false
    this.dispatchEvent(createDetachedEvent(this))
  }

  /**
   * @ignore
   */
  $destroy () {
    const { _sample, _stream } = this

    _stream?.$destroy()
    _sample?.$destroy()

    this._isAttached = false
    this._sample = this._stream = this._plugin = null
  }
}
