/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

namespace lse {

inline bool Equals(const float a, const float b) noexcept { return std::abs(a - b) < 0.0001f; }

/**
 * Snaps a float value to the screen's pixel grid.
 */
template <typename T>
T SnapToPixelGrid(float f) noexcept;

template <>
inline int32_t SnapToPixelGrid(float f) noexcept {
    return std::isnan(f) ? f : f < 0.f ? f - 0.5f : f + 0.5f;
}

template <>
inline float SnapToPixelGrid(float f) noexcept {
    return SnapToPixelGrid<int32_t>(f);
}

constexpr auto kUndefined = std::numeric_limits<float>::quiet_NaN();

} // namespace lse
