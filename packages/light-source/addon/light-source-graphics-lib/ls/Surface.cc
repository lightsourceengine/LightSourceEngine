/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Surface.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <ls/Endian.h>
#include "PixelConversion.h"

namespace ls {

Surface::Surface(std::shared_ptr<uint8_t> pixels, const int32_t width, const int32_t height) noexcept
: width(std::max(width, 0)), height(std::max(height, 0)), pixels(pixels), format(PixelFormatAlpha)  {
    if (this->width == 0 || this->height == 0 || !this->pixels) {
        this->width = this->height = this->pitch = 0;
        this->pixels = nullptr;
    } else {
        this->pitch = this->width;
    }
}

Surface::Surface(std::shared_ptr<uint8_t> pixels, const int32_t width, const int32_t height,
    const int32_t pitch, const PixelFormat format) noexcept
: width(std::max(width, 0)), height(std::max(height, 0)), pitch(std::max(pitch, 0)), pixels(pixels), format(format)  {
    if (this->width == 0 || this->height == 0 || !this->pixels || format == PixelFormatUnknown) {
        this->width = this->height = this->pitch = 0;
        this->pixels = nullptr;
    } else if (this->pitch == 0) {
        this->pitch = GetComponentCount(this->format) * width;
    }
}

Surface::Surface(Surface&& other) noexcept {
    this->pixels = std::move(other.pixels);
    this->width = other.width;
    this->height = other.height;
    this->pitch = other.pitch;
    this->format = other.format;
}

void Surface::Blit(const int32_t x, const int32_t y, const Surface& surface) const noexcept {
    if (this->IsEmpty()) {
        return;
    }

    // For now, only a single channel source is supported.
    assert(surface.format == PixelFormatAlpha);

    // intersect this with surface arg
    const auto x1{ std::max(x, 0) };
    const auto x2{ std::min(x + surface.width, this->width) };
    const auto y1{ std::max(y, 0) };
    const auto y2{ std::min(y + surface.height, this->height) };
    const auto spitch{ x2 - x1 };

    // bail if no intersection
    if (y2 - y1 <= 0 || spitch <= 0) {
        return;
    }

    // if a part of surface is outside of the bounds of this, clip it.
    auto sx{ x < 0 ? std::abs(x) : 0 };
    auto sy{ y < 0 ? std::abs(y) : 0 };

    // sanity check that source x & y are within source dimensions
    assert(sx < surface.width && sy < surface.height);

    const auto dest{ this->pixels.get() };
    const auto source{ surface.Pixels() };
    const auto componentCount{ GetComponentCount(this->format) };

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
                const auto component{ *sourceRow++ }; // NOLINT(readability/pointer_notation)

                for (int32_t i{ 0 }; i < componentCount; i++) {
                    *destRow++ = component;
                }
            }
        }
    }
}

void Surface::FillTransparent() const noexcept {
    if (this->IsEmpty()) {
        return;
    }

    std::memset(this->pixels.get(), 0, this->pitch * this->height);
}

void Surface::Convert(const PixelFormat format) noexcept {
    assert(GetComponentCount(format) == 4);

    if (this->IsEmpty()) {
        return;
    }

    if (this->format != (IsBigEndian() ? PixelFormatRGBA : PixelFormatABGR)) {
        return;
    }

    auto buffer{ reinterpret_cast<Color*>(this->Pixels()) };
    const auto len{ (this->height * this->pitch) / sizeof(Color) };

    ConvertToFormat(buffer, len, format);

    this->format = format;
}

Surface& Surface::operator=(Surface&& other) noexcept {
    this->pixels = std::move(other.pixels);
    this->width = other.width;
    this->height = other.height;
    this->pitch = other.pitch;
    this->format = other.format;

    return *this;
}

} // namespace ls
