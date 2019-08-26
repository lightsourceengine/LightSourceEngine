/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Surface.h"
#include <algorithm>

namespace ls {

constexpr int GetComponentCount(PixelFormat format) {
    switch (format) {
        case PixelFormatRGBA:
        case PixelFormatARGB:
        case PixelFormatABGR:
        case PixelFormatBGRA:
            return 4;
        case PixelFormatAlpha:
            return 1;
        case PixelFormatUnknown:
            return 0;
    }
}

Surface::Surface(int32_t width, int32_t height) : format(PixelFormatAlpha) {
    if (width <= 0 || height <= 0) {
        this->width = this->height = this->pitch = 0;
    } else {
        this->width = width;
        this->height = height;
        this->pitch = width;
        this->pixels = std::shared_ptr<uint8_t>(new uint8_t[width * height]{0}, [](uint8_t* data){ delete [] data; });
    }
}

Surface::Surface(std::shared_ptr<uint8_t> pixels, int32_t width, int32_t height)
: width(width), height(height), pixels(pixels), format(PixelFormatAlpha)  {
    if (width <= 0 || height <= 0 || !this->pixels) {
        this->width = this->height = this->pitch = 0;
        this->pixels = nullptr;
    }

    this->pitch = this->width * GetComponentCount(format);
}

Surface::Surface(std::shared_ptr<uint8_t> pixels, int32_t width, int32_t height, int32_t pitch, PixelFormat format)
: width(width), height(height), pitch(pitch), pixels(pixels), format(format)  {
    if (width <= 0 || height <= 0 || !this->pixels) {
        this->width = this->height = 0;
        this->pixels = nullptr;
    }
}

Surface::Surface(Surface&& other) {
    this->pixels = std::move(other.pixels);
    this->width = other.width;
    this->height = other.height;
    this->pitch = other.pitch;
    this->format = other.format;
}

void Surface::Blit(const int32_t x, const int32_t y, const Surface& surface) const {
    if (this->IsEmpty()) {
        return;
    }

    // For now, only a single channel source is supported.
    assert(surface.format == PixelFormatAlpha);

    // intersect this with surface arg
    auto x1{ std::max(x, 0) };
    auto x2{ std::min(x + surface.width, this->width) };
    auto y1{ std::max(y, 0) };
    auto y2{ std::min(y + surface.height, this->height) };
    auto spitch{ x2 - x1 };

    // bail if no intersection
    if (y2 - y1 <= 0 || spitch <= 0) {
        return;
    }

    // if a part of surface is outside of the bounds of this, clip it.
    auto sx{ x < 0 ? std::abs(x) : 0 };
    auto sy{ y < 0 ? std::abs(y) : 0 };

    // sanity check that source x & y are within source dimensions
    assert(sx < surface.width && sy < surface.height);

    auto dest{ this->pixels.get() };
    auto source{ surface.Pixels() };
    auto componentCount{ GetComponentCount(this->format) };

    // copy pixels from source to dest
    if (componentCount == 1) {
        // single chanel copy
        for (auto dy{ y1 }; dy < y2; dy++, sy++) {
            std::memcpy(&dest[dy * this->pitch + x1], &source[(sy * surface.pitch) + sx], spitch);
        }
    } else if (componentCount == 4) {
        // assume source is a single channel surface so endianness can be ignored
        for (auto dy{ y1 }; dy < y2; dy++, sy++) {
            auto destRow{ &dest[(dy * this->pitch) + (x1 * componentCount)] };
            auto sourceRow{ &source[(sy * surface.pitch) + sx] };

            for (int32_t dx{ 0 }; dx < spitch; dx++) {
                auto component{ *sourceRow++ }; // NOLINT(readability/pointer_notation)

                for (int32_t i{0}; i < componentCount; i++) {
                    *destRow++ = component;
                }
            }
        }
    }
}

void Surface::FillTransparent() {
    if (this->IsEmpty()) {
        return;
    }

    auto componentCount{ GetComponentCount(this->format) };

    if (componentCount * this->width == this->pitch) {
        std::memset(this->pixels.get(), 0, this->pitch*this->height);
    } else {
        auto source{ this->pixels.get() };

        for (int32_t y{0}; y < this->height; y++) {
            std::memset(&source[y*this->pitch], 0, this->pitch);
        }
    }
}

} // namespace ls
