/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "PixelConversion.h"
#include <ls/Endian.h>

namespace ls {

template<int32_t C1, int32_t C2, int32_t C3, int32_t C4>
void Convert(Color* pixels, const uint32_t len) noexcept {
    for (uint32_t i = 0; i < len; i++) {
        const Color color{ pixels[i] };

        pixels[i].value = (color.channels[C1] << 24)
            | (color.channels[C2] << 16)
            | (color.channels[C3] << 8)
            | color.channels[C4];
    }
}

void ToFormatLE(Color* pixels, const int32_t len, const PixelFormat format) noexcept {
    // Mapping for PixelFormatABGR
    constexpr auto R = 0;
    constexpr auto G = 1;
    constexpr auto B = 2;
    constexpr auto A = 3;

    switch (format) {
        case PixelFormatARGB:
            Convert<A, R, G, B>(pixels, len);
            break;
        case PixelFormatBGRA:
            Convert<B, G, R, A>(pixels, len);
            break;
        case PixelFormatRGBA:
            Convert<R, G, B, A>(pixels, len);
            break;
        default:
            // PixelFormatABGR - no op in LE
            break;
    }
}

void ToFormatBE(Color* pixels, const int32_t len, const PixelFormat format) noexcept {
    // Mapping for PixelFormatRGBA
    constexpr auto R = 3;
    constexpr auto G = 2;
    constexpr auto B = 1;
    constexpr auto A = 0;

    switch (format) {
        case PixelFormatARGB:
            Convert<A, R, G, B>(pixels, len);
            break;
        case PixelFormatBGRA:
            Convert<B, G, R, A>(pixels, len);
            break;
        case PixelFormatABGR:
            Convert<A, B, G, R>(pixels, len);
            break;
        default:
            // PixelFormatRGBA - no op in BE
            break;
    }
}

void ConvertToFormat(Color* pixels, const int32_t len, const PixelFormat format) noexcept {
    if (IsBigEndian()) {
       ToFormatBE(pixels, len, format);
    } else {
       ToFormatLE(pixels, len, format);
    }
}

} // namespace ls
