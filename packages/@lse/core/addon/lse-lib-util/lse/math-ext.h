/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

namespace lse {

using Float266 = int32_t;
using Float1616 = int32_t;

inline bool Equals(float a, float b) noexcept {
  return std::abs(a - b) < 0.0001f;
}

template<typename T>
T SnapToPixelGrid(float f) noexcept;

template<>
inline int32_t SnapToPixelGrid(float f) noexcept {
  return std::isnan(f) ? f : f < 0.f ? f - 0.5f : f + 0.5f;
}

template<>
inline float SnapToPixelGrid(float f) noexcept {
  return SnapToPixelGrid<int32_t>(f);
}

//inline constexpr float FromFloat266(signed long value) noexcept {
//  // 26.6 format to float
//  return static_cast<float>(value) / static_cast<float>(0b111111);
//}

inline constexpr float FromFloat266(Float266 value) noexcept {
  // 26.6 format to float
  return static_cast<float>(value) / static_cast<float>(0b111111);
}

inline constexpr Float266 ToFloat266(float value) noexcept {
  return static_cast<int32_t>(value * static_cast<float>(0b111111));
}

inline constexpr Float266 ToFloat266(std::size_t value) noexcept {
  return static_cast<int32_t>(value) << 6;
}

//inline constexpr float FromFloat1616(signed long value) noexcept {
//  // 16.16 format to float
//  return static_cast<float>(value) / static_cast<float>(0xFFFF);
//}

inline constexpr float FromFloat1616(Float1616 value) noexcept {
  // 16.16 format to float
  return static_cast<float>(value) / static_cast<float>(0xFFFF);
}

constexpr auto kUndefined = std::numeric_limits<float>::quiet_NaN();

} // namespace lse
