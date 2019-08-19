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
    // intersect this with surface arg
    auto x1{ std::max(x, 0) };
    auto x2{ std::min(x + surface.width, this->width) };
    auto y1{ std::max(y, 0) };
    auto y2{ std::min(y + surface.height, this->height) };

    // bail if no intersection
    if (y2 - y1 <= 0 || x2 - x1 <= 0) {
        return;
    }

    // if a part of surface is outside of the bounds of this, clip it.
    auto sx{ x < 0 ? std::abs(x) : 0 };
    auto sy{ y < 0 ? std::abs(y) : 0 };
    auto spitch{ x2 - x1 };

    auto dest{ this->pixels.get() };
    auto source{ surface.Pixels() };

    for (auto dy{ y1 }; dy < y2; dy++, sy++) {
        std::memcpy(&dest[dy * this->width + x1], &source[(sy * surface.width) + sx], spitch);
    }
}

} // namespace ls
