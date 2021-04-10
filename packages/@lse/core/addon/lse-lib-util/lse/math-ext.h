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
