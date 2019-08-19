/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>

namespace ls {

class Surface {
 public:
    Surface(int32_t width, int32_t height);
    Surface(std::shared_ptr<uint8_t> pixels, int32_t width, int32_t height);
    explicit Surface(Surface&& other);

    void Blit(const int32_t x, const int32_t y, const Surface& surface) const;

    bool IsEmpty() const { return this->width == 0 || this->height == 0; }
    int32_t Width() const { return this->width; }
    int32_t Height() const { return this->height; }
    uint8_t* Pixels() const { return this->pixels.get(); }

 private:
    int32_t width;
    int32_t height;
    mutable std::shared_ptr<uint8_t> pixels;
};

} // namespace ls
