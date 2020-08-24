/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Values returned by AudioSource.type() to describe the kind of AudioSource.
 *
 * @type {Readonly<{Null: string, Sample: string, Stream: string}>}
 */
export const AudioSourceType = Object.freeze({
  /**
   * Audio source that is just-in-time decoded to continuously playback. Streams are used for longer music playback,
   * such as background music or songs.
   */
  Stream: 'stream',

  /**
   * AudioSource that is read and decoded into memory at load time. Samples should be used for sound effects.
   */
  Sample: 'sample',

  /**
   * AudioSource that will not load or play audio. This type is substituted when an AudioSource cannot be found.
   */
  Null: 'null'
})
