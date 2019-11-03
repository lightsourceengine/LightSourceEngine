/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <tuple>
#include <memory>
#include <stb_truetype.h>

namespace ls {

class FontResource;

class Font {
 public:
    Font(std::unique_ptr<uint8_t[]>&& ttf, int32_t index);

    float Ascent(float fontSize) const noexcept;
    float LineHeight(float fontSize) const noexcept;
    float Kerning(uint32_t cp, uint32_t next, float fontSize) const noexcept;
    float Advance(uint32_t  cp, float fontSize) const noexcept;
    bool Exists(uint32_t cp) const noexcept;
    stbtt_fontinfo* Info() const noexcept { return &this->info; }
    bool IsReady() const noexcept { return !!this->ttf; }

 private:
    mutable stbtt_fontinfo info;
    std::unique_ptr<uint8_t[]> ttf;
    float ascent;
    float lineHeight;
};

} // namespace ls
