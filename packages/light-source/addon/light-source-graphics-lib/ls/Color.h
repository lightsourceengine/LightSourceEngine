/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <cstdint>
#include <std17/algorithm>
#include <ls/ByteOrder.h>

namespace ls {

/**
 * 32-bit color type. 8-bits per channel. 0xAARRGGBB format.
 */
union color_t {
    uint32_t value;
    uint8_t channels[4];

    struct {
#if LS_BYTE_ORDER == LS_LITTLE_ENDIAN
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
#else
        uint8_t a;
        uint8_t r;
        uint8_t g;
        uint8_t b;
#endif
    };

    constexpr color_t() noexcept : value(0u) {}
    constexpr color_t(uint32_t color) noexcept : value(color) {}

// XXX: work around compiler initialization order warnings
#if LS_BYTE_ORDER == LS_LITTLE_ENDIAN
    constexpr color_t(uint32_t a, uint32_t r, uint32_t g, uint32_t b) noexcept
        : b(b & 0xFFu), g(g & 0xFFu), r(r & 0xFFu), a(a & 0xFFu) {}
#else
    constexpr color_t(uint32_t a, uint32_t r, uint32_t g, uint32_t b) noexcept
            : a(a & 0xFFu), r(r & 0xFFu), g(g & 0xFFu), b(b & 0xFFu) {}
#endif

    constexpr bool operator==(const color_t& color) const noexcept { return this->value == color.value; }
    constexpr bool operator!=(const color_t& color) const noexcept { return this->value != color.value; }

    /**
     * Multiples the alpha component by an opacity value.
     *
     * @param opacity Value between [0-1]
     * @return multiplied color value
     */
    constexpr color_t MixAlpha(float opacity) const noexcept {
        return {
            static_cast<uint32_t>(static_cast<float>(this->a) * std17::clamp(opacity, 0.f, 1.f)),
            this->r,
            this->g,
            this->b
        };
    }
};

static_assert(sizeof(color_t) == 4, "color_t union must be 4 bytes!");

constexpr color_t ColorWhite = 0xFFFFFFFF;
constexpr color_t ColorBlack = 0xFF000000;
constexpr color_t ColorTransparent = 0;

} // namespace ls
