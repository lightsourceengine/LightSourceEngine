/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <ls/EnumSequence.h>

namespace ls {

/**
 * Pixel formats used for texture creation.
 */
LS_ENUM_SEQ_DECL(
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

} // namespace ls
