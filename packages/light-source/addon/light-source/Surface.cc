/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Surface.h"
#include <algorithm>

namespace ls {

Surface::Surface(int32_t width, int32_t height) {
    if (width <= 0 || height <= 0) {
        this->width = this->height = 0;
    } else {
        this->width = width;
        this->height = height;
        this->pixels = std::shared_ptr<uint8_t>(new uint8_t[width * height]{0}, [](uint8_t* data){ delete [] data; });
    }
}

Surface::Surface(std::shared_ptr<uint8_t> pixels, int32_t width, int32_t height)
: width(width), height(height), pixels(pixels) {
    if (width <= 0 || height <= 0 || !this->pixels) {
        this->width = this->height = 0;
        this->pixels = nullptr;
    }
}

Surface::Surface(Surface&& other) {
    this->pixels = std::move(other.pixels);
    this->width = other.width;
    this->height = other.height;
}

void Surface::Blit(const int32_t x, const int32_t y, const Surface& surface) const {
    if (surface.IsEmpty()) {
        return;
    }

    // TODO: negative?
    auto x1{ std::max(x, 0) };
    auto x2{ std::min(x + surface.width, this->width) };

    // TODO: negative?
    auto y1{ std::max(y, 0) };
    auto y2{ std::min(y + surface.height, this->height) };

    if (y2 - y1 <= 0 || x2 - x1 <= 0) {
        return;
    }

    auto dest{ this->Pixels() };
    auto source{ surface.Pixels() };
    auto copyWidth{ x2 - x1 };

    int32_t s = 0;

    // TODO: source x, source y

    for (int32_t d = y1; d < y2; d++) {
        // TODO: source might be clipped..
        std::memcpy(&dest[d * this->width + x1], &source[s * surface.width], copyWidth);
        s++;
    }
}

} // namespace ls
