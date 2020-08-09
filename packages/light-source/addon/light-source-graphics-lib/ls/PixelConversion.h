/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/PixelFormat.h>
#include <cstdint>
#include <std20/bit>

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

constexpr uint8_t GetR(const uint32_t color) noexcept {
    return (std20::endian::native == std20::endian::big) ? ((color & 0xFF00) >> 8u) : ((color & 0xFF0000) >> 16u);
}

constexpr uint8_t GetG(const uint32_t color) noexcept {
    return (std20::endian::native == std20::endian::big) ? ((color & 0xFF0000) >> 16u) : ((color & 0xFF00) >> 8u);
}

constexpr uint8_t GetB(const uint32_t color) noexcept {
    return (std20::endian::native == std20::endian::big) ? ((color & 0xFF000000) >> 24u) : (color & 0xFF);
}

constexpr uint8_t GetA(const uint32_t color) noexcept {
    return (std20::endian::native == std20::endian::big) ? (color & 0xFF) : ((color & 0xFF000000) >> 24u);
}

constexpr uint32_t ARGB(const uint8_t a, const uint8_t r, const uint8_t g, const uint8_t b) noexcept {
    return (std20::endian::native == std20::endian::big) ?
        (b << 24u) | (g << 16u) | (r << 8u) | a
        : ((a << 24u) | (r << 16u) | (g << 8u) | b);
}

constexpr uint32_t RGB(const uint8_t r, const uint8_t g, const uint8_t b) noexcept {
    return ARGB(0xFF, r, g, b);
}

constexpr uint32_t MixAlpha(const uint32_t color, const float opacity) noexcept {
    return ARGB(
        static_cast<uint8_t>(GetA(color) * opacity),
        GetR(color),
        GetG(color),
        GetB(color));
}

constexpr uint32_t ColorWhite = 0xFFFFFFFF;
constexpr uint32_t ColorBlack = 0xFF000000;
constexpr uint32_t ColorTransparent = 0;

} // namespace ls
