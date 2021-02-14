/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <vector>
#include <lse/EnumSequence.h>

namespace lse {

struct AudioPluginConfig {
  AudioPluginConfig() = default;
  AudioPluginConfig(std::string&& audioFormat, int32_t frequency, int32_t bytesPerSample, int32_t channels);
  AudioPluginConfig(std::string&& audioFormat, int32_t frequency, int32_t bytesPerSample, int32_t channels,
                    std::vector<std::string>&& decoders);

  std::string audioFormat{};
  int32_t frequency{44100};
  int32_t bytesPerSample{4096};
  int32_t channels{2};
  std::vector<std::string> decoders;
};

} // namespace lse
