/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cmath>
#include <cstdint>

inline bool Equals(const float a, const float b) noexcept {
    return std::abs(a - b) < 0.0001f;
}

/**
 * Snaps a float value to the screen's pixel grid.
 */
inline float SnapToPixelGrid(const float f) noexcept {
    if (std::isnan(f)) {
        return f;
    }

    return static_cast<int32_t>(f < 0 ? f - 0.5f : f + 0.5f);
}
