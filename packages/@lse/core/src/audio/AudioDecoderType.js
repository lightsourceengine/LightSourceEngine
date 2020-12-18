/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

/**
 * List of supported audio codecs. Used by AudioDestination.hasDecoder() to test if a sound file can be loaded.
 *
 * The values are pulled from SDL Mixer (and SDL Audio). Refer to SDL 2 documentation for information about which
 * file types each decoder can load.
 *
 * @type {Readonly<{MODPLUG: string, MOD: string, MIDI: string, FLAC: string, TIMIDITY: string, VOC: string,
 * OGG: string, MP3: string, WAVE: string, AIFF: string, OPUS: string, CMD: string, MIKMOD: string}>}
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
