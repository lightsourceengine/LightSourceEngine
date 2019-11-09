/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/PixelFormat.h>
#include <SDL.h>

namespace ls {

constexpr PixelFormat ToPixelFormat(Uint32 pixelFormat) {
    switch (pixelFormat) {
        case SDL_PIXELFORMAT_ARGB8888:
            return PixelFormatARGB;
        case SDL_PIXELFORMAT_RGBA8888:
            return PixelFormatRGBA;
        case SDL_PIXELFORMAT_ABGR8888:
            return PixelFormatABGR;
        case SDL_PIXELFORMAT_BGRA8888:
            return PixelFormatBGRA;
        default:
            return PixelFormatUnknown;
    }
}

} // namespace ls
