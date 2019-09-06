/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

export const AudioSourceCapabilityVolume = 0
export const AudioSourceCapabilityLoop = 1
export const AudioSourceCapabilityFadeIn = 2

export const AudioDestinationCapabilityStop = 0
export const AudioDestinationCapabilityResume = 1
export const AudioDestinationCapabilityPause = 2
export const AudioDestinationCapabilityVolume = 3
export const AudioDestinationCapabilityFadeOut = 4

export const AudioSourceStateInit = 0
export const AudioSourceStateLoading = 1
export const AudioSourceStateReady = 2
export const AudioSourceStateError = 3

export const AudioSourceTypeStream = 'stream'
export const AudioSourceTypeSample = 'sample'
