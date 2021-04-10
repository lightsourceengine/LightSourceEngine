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
