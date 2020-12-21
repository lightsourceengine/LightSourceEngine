/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <std20/bit>

#include "Color.h"
#include "PixelFormat.h"

namespace lse {

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
void ConvertToFormat(color_t* pixels, int32_t len, PixelFormat format) noexcept;

} // namespace lse
