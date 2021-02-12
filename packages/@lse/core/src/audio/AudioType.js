/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * Audio destination buffer type.
 *
 * The type is used by AudioDestination and any AudioSources associated with a destination buffer.
 *
 * @type {Readonly<{Null: string, Sample: string, Stream: string}>}
 */
export const AudioType = Object.freeze({
  /**
   * Destination buffer that supports the playback of a single audio source. The buffer is intended for
   * playback of background music.
   */
  STREAM: 'stream',

  /**
   * Destination buffer that supports the simultaneous playback of multiple, short audio sources. The buffer
   * is intended for playback of sound effects.
   */
  SAMPLE: 'sample',

  /**
   * Destination buffer that is uninitialized or does not support playback.
   */
  NULL: 'null'
})
