/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <vector>

namespace lse {

float ConstrainVolume(const float volume) noexcept;
std::vector<std::string> ListDecoders(int32_t count, const char*(*toString)(int32_t index));

} // namespace lse
