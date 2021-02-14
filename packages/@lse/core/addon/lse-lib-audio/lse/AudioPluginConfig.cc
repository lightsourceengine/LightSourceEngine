/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "AudioPluginConfig.h"

#include <cstring>

namespace lse {

AudioPluginConfig::AudioPluginConfig(
    std::string&& audioFormat, int32_t frequency, int32_t bytesPerSample, int32_t channels) {
  this->audioFormat = std::move(audioFormat);

  if (frequency > 0) {
    this->frequency = frequency;
  }

  if (bytesPerSample > 0 && (bytesPerSample & (bytesPerSample - 1))) {
    this->bytesPerSample = bytesPerSample;
  }

  if (channels > 0) {
    this->channels = channels;
  }
}

AudioPluginConfig::AudioPluginConfig(std::string&& audioFormat, int32_t frequency, int32_t bytesPerSample,
    int32_t channels, std::vector<std::string>&& decoders)
: AudioPluginConfig(std::move(audioFormat), frequency, bytesPerSample, channels) {
  this->decoders = std::move(decoders);
}

} // namespace lse
