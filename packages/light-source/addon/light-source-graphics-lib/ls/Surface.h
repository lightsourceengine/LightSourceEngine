/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/PixelFormat.h>
#include <memory>

namespace ls {

class Surface {
 public:
    Surface() noexcept = default;
    Surface(std::shared_ptr<uint8_t> pixels, const int32_t width, const int32_t height) noexcept;
    Surface(std::shared_ptr<uint8_t> pixels, const int32_t width, const int32_t height, const int32_t pitch,
        const PixelFormat format) noexcept;
    Surface(Surface&& other) noexcept;

    void Blit(const int32_t x, const int32_t y, const Surface& surface) const noexcept;
    void FillTransparent() const noexcept;
    void Convert(const PixelFormat format) noexcept;

    bool IsEmpty() const noexcept { return !this->pixels; }
    int32_t Width() const noexcept { return this->width; }
    int32_t Height() const noexcept { return this->height; }
    int32_t Pitch() const noexcept { return this->pitch; }
    uint8_t* Pixels() const noexcept { return this->pixels.get(); }
    PixelFormat Format() const noexcept { return this->format; }

    Surface& operator=(Surface&& other) noexcept;

 private:
    int32_t width{};
    int32_t height{};
    int32_t pitch{};
    std::shared_ptr<uint8_t> pixels;
    PixelFormat format{};
};

} // namespace ls
