/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <ls/PixelFormat.h>

namespace ls {

union Color {
    uint32_t value;
    uint8_t channels[4];
};

static_assert(sizeof(Color) == 4, "Color union must be 4 bytes!");

/**
 * In place conversion of a buffer of RGBA color values to the specified pixel format.
 *
 * The format of the pixel buffer must be PixelFormatRGBA (BE) or PixelFormatABGR (LE). If another format, the
 * behavior is undefined.
 *
 * @param pixels Buffer containing a list of 4 bytes pixels.
 * @param len Number of pixels in the buffer.
 * @param format Pixel format to convert to
 */
void ConvertToFormat(Color* pixels, const int32_t len, const PixelFormat format) noexcept;

} // namespace ls
