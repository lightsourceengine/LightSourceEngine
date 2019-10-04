/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <Renderer.h>
#include <memory>

namespace ls {

class Surface {
 public:
    Surface(int32_t width, int32_t height);
    Surface(std::shared_ptr<uint8_t> pixels, int32_t width, int32_t height);
    Surface(std::shared_ptr<uint8_t> pixels, int32_t width, int32_t height, int32_t pitch, PixelFormat format);
    explicit Surface(Surface&& other);

    void Blit(const int32_t x, const int32_t y, const Surface& surface) const;
    void FillTransparent() const;

    bool IsEmpty() const { return this->width == 0 || this->height == 0; }
    int32_t Width() const { return this->width; }
    int32_t Height() const { return this->height; }
    int32_t Pitch() const { return this->pitch; }
    uint8_t* Pixels() const { return this->pixels.get(); }
    PixelFormat Format() const { return this->format; }

 private:
    int32_t width;
    int32_t height;
    int32_t pitch;
    mutable std::shared_ptr<uint8_t> pixels;
    PixelFormat format;
};

} // namespace ls
