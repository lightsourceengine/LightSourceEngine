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
 * List of supported audio codecs. Used by AudioDestination.hasDecoder() to test if a sound file can be loaded.
 *
 * The values are pulled from SDL Mixer (and SDL Audio). Refer to SDL 2 documentation for information about which
 * file types each decoder can load.
 *
 * @enum {string}
 * @readonly
 * @name module:@lse/core.core-enum.AudioDecoderType
 */
export const AudioDecoderType = Object.freeze({
  AIFF: 'AIFF',
  CMD: 'CMD',
  FLAC: 'FLAC',
  MIDI: 'MIDI',
  MIKMOD: 'MIKMOD',
  MOD: 'MOD',
  MODPLUG: 'MODPLUG',
  MP3: 'MP3',
  OGG: 'OGG',
  OPUS: 'OPUS',
  TIMIDITY: 'TIMIDITY',
  VOC: 'VOC',
  WAVE: 'WAVE'
})
