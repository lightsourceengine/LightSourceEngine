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

#include <cstdint>
#include <lse/EnumSequence.h>

namespace lse {

/**
 * Pixel formats used for texture creation.
 */
LSE_ENUM_SEQ_DECL(
    PixelFormat,
/* 4 channel, 32 bit, RGBA */
    PixelFormatRGBA,
/* 4 channel, 32 bit, ARGB */
    PixelFormatARGB,
/* 4 channel, 32 bit, ABGR */
    PixelFormatABGR,
/* 4 channel, 32 bit, BGRA */
    PixelFormatBGRA,
/* 1 channel, 8 bit, alpha */
    PixelFormatAlpha,
/* unknown pixel format */
    PixelFormatUnknown
)

constexpr int32_t GetComponentCount(const PixelFormat format) noexcept {
  switch (format) {
    case PixelFormatRGBA:
    case PixelFormatARGB:
    case PixelFormatABGR:
    case PixelFormatBGRA:
      return 4;
    case PixelFormatAlpha:
      return 1;
      // case PixelFormatUnknown:
    default:
      return 0;
  }
}

} // namespace lse
