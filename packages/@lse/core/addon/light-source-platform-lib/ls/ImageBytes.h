/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

namespace ls {

class ImageBytes {
 public:
    using Deleter = void(*)(uint8_t*);

 public:
    ImageBytes() noexcept = default;

    ImageBytes(uint8_t* bytes, Deleter deleter, int32_t width, int32_t height, int32_t pitch)
        : bytes(bytes, deleter), width(width), height(height), pitch(pitch) {
    }

    int32_t Width() const noexcept {
        return this->width;
    }

    int32_t Height() const noexcept {
        return this->height;
    }

    float WidthF() const noexcept {
        return this->width;
    }

    float HeightF() const noexcept {
        return this->height;
    }

    int32_t Pitch() const noexcept {
        return this->pitch;
    }

    uint8_t* Bytes() const noexcept {
        return this->bytes.get();
    }

    void Release() {
        this->bytes.reset();
        this->width = this->height = this->pitch = 0;
    }

 private:
    std::shared_ptr<uint8_t> bytes;
    int32_t width;
    int32_t height;
    int32_t pitch;
};

} // namespace ls