/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "PixelConversion.h"
#include <utility>

constexpr auto R{ 0 };
constexpr auto G{ 1 };
constexpr auto B{ 2 };
constexpr auto A{ 3 };
constexpr auto NUM_IMAGE_COMPONENTS{ 4 };

namespace ls {

inline
void ToFormatLE(uint8_t* bytes, int32_t len, PixelFormat format) {
    auto i{ 0 };
    uint8_t r, g, b, a;

    switch (format) {
        case PixelFormatARGB:
            while (i < len) {
                std::swap(bytes[i + R], bytes[i + B]);
                i += NUM_IMAGE_COMPONENTS;
            }
            break;
        case PixelFormatBGRA:
            while (i < len) {
                r = bytes[i + R];
                g = bytes[i + G];
                b = bytes[i + B];
                a = bytes[i + A];

                bytes[i    ] = a;
                bytes[i + 1] = r;
                bytes[i + 2] = g;
                bytes[i + 3] = b;

                i += NUM_IMAGE_COMPONENTS;
            }
            break;
        case PixelFormatRGBA:
            while (i < len) {
                std::swap(bytes[i + R], bytes[i + A]);
                std::swap(bytes[i + G], bytes[i + B]);

                i += NUM_IMAGE_COMPONENTS;
            }
            break;
        default:
            // TEXTURE_FORMAT_ABGR - no op in LE
            break;
    }
}

inline
void ToFormatBE(uint8_t* bytes, int32_t len, PixelFormat format) {
    auto i{ 0 };
    uint8_t r, g, b, a;

    switch (format) {
        case PixelFormatABGR:
            while (i < len) {
                std::swap(bytes[i + A], bytes[i + R]);
                std::swap(bytes[i + G], bytes[i + B]);
                i += NUM_IMAGE_COMPONENTS;
            }
            break;
        case PixelFormatARGB:
            while (i < len) {
                r = bytes[i + R];
                g = bytes[i + G];
                b = bytes[i + B];
                a = bytes[i + A];

                bytes[i    ] = a;
                bytes[i + 1] = r;
                bytes[i + 2] = g;
                bytes[i + 3] = b;

                i += NUM_IMAGE_COMPONENTS;
            }
            break;
        case PixelFormatBGRA:
            while (i < len) {
                std::swap(bytes[i + R], bytes[i + B]);
                i += NUM_IMAGE_COMPONENTS;
            }
            break;
        case PixelFormatRGBA:
        default:
            // TEXTURE_FORMAT_RGBA - noop in BE
            break;
    }
}

void ConvertToFormat(uint8_t* bytes, int32_t len, PixelFormat format) {
    if (IsBigEndian()) {
       ToFormatBE(bytes, len, format);
    } else {
       ToFormatLE(bytes, len, format);
    }
}

} // namespace ls
