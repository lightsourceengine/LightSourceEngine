/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

namespace ls {

/**
 * Pixel formats used for texture creation.
 */
enum PixelFormat {
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
};

} // namespace ls
