/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <ls/PixelFormat.h>

namespace ls {

// Implementation from C++20 endian proposal: http://howardhinnant.github.io/endian.html
enum class endian {
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

/**
 * Check if ths CPU is big endian.
 */
constexpr bool IsBigEndian() {
    return endian::native == endian::big;
}

/**
 * In place conversion of a buffer of RGBA color values to the specified pixel format.
 *
 * @param bytes Byte buffer containing RGBA formatted color values
 * @param len Size of bytes in bytes
 * @param format Pixel format to convert to
 */
void ConvertToFormat(uint8_t* bytes, int32_t len, PixelFormat format);

} // namespace ls
