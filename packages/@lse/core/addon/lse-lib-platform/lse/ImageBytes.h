/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <memory>
#include <lse/PixelConversion.h>

namespace lse {

class ImageBytes {
 public:
  using Deleter = void (*)(uint8_t*);

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

  PixelFormat Format() const noexcept {
    return this->format;
  }

  void SyncFormat(PixelFormat targetFormat) noexcept {
    if (targetFormat == PixelFormat::PixelFormatUnknown || targetFormat == PixelFormat::PixelFormatRGBA
        || targetFormat == this->format || !this->bytes) {
      return;
    }

    ConvertToFormat(
        reinterpret_cast<color_t*>(this->bytes.get()),
        this->width * this->height,
        targetFormat);

    this->format = targetFormat;
  }

  void Release() {
    this->bytes.reset();
    this->width = this->height = this->pitch = 0;
  }

 private:
  std::shared_ptr<uint8_t> bytes{};
  int32_t width{};
  int32_t height{};
  int32_t pitch{};
  PixelFormat format{PixelFormat::PixelFormatRGBA};
};

} // namespace lse
