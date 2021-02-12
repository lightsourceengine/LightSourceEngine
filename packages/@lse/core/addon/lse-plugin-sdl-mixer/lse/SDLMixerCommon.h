/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>

namespace lse {

constexpr auto MIX_MAX_VOLUME_F{static_cast<float>(MIX_MAX_VOLUME)};
constexpr auto ALL_CHANNELS{-1};

} // namespace lse
