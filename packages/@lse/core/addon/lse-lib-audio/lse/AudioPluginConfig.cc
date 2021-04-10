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
