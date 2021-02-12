/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "AudioUtil.h"
#include <std17/algorithm>

namespace lse {

float ConstrainVolume(const float volume) noexcept {
  return std17::clamp(volume, 0.f, 1.f);
}

std::vector<std::string> ListDecoders(int32_t count, const char*(*toString)(int32_t index)) {
  std::vector<std::string> decoders;

  decoders.reserve(count);

  for (int32_t i = 0; i < count; i++) {
    auto name{toString(i)};
    decoders.emplace_back(name ? name : "unknown");
  }

  return decoders;
}

} // namespace lse
