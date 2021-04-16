/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

/**
 * Audio destination buffer type.
 *
 * The type is used by AudioDestination and any AudioSources associated with a destination buffer.
 *
 * @enum {string}
 * @readonly
 * @name module:@lse/core.core-enum.AudioType
 * @property {string} SAMPLE ...
 * @property {string} STREAM ...
 * @property {string} NULL ...
 */
export const AudioType = Object.freeze({
  // /**
  //  * Destination buffer that supports the playback of a single audio source. The buffer is intended for
  //  * playback of background music.
  //  */
  STREAM: 'stream',

  // /**
  //  * Destination buffer that supports the simultaneous playback of multiple, short audio sources. The buffer
  //  * is intended for playback of sound effects.
  //  */
  SAMPLE: 'sample',

  // /**
  //  * Destination buffer that is uninitialized or does not support playback.
  //  */
  NULL: 'null'
})
