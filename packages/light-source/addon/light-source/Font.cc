/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Font.h"

namespace ls {

Font::Font(std::unique_ptr<uint8_t[]>&& ttf, int32_t index) {
    const auto offset{ stbtt_GetFontOffsetForIndex(ttf.get(), index) };

    if (offset == -1) {
        throw std::runtime_error("Cannot find font at index.");
    }

    if (stbtt_InitFont(&this->info, ttf.get(), offset) == 0) {
        throw std::runtime_error("Failed to init font.");
    }

    int32_t ascent{0};
    int32_t descent{0};
    int32_t lineGap{0};

    stbtt_GetFontVMetrics(&this->info, &ascent, &descent, &lineGap);

    this->ascent = ascent;
    this->lineHeight = static_cast<float>(ascent - descent + lineGap);
    this->ttf = std::move(ttf);
}

float Font::Ascent(float fontSize) const noexcept {
    return this->ascent * stbtt_ScaleForPixelHeight(&this->info, fontSize);
}

float Font::LineHeight(float fontSize) const noexcept {
    return this->lineHeight * stbtt_ScaleForPixelHeight(&this->info, fontSize);
}

float Font::Kerning(uint32_t cp, uint32_t next, float fontSize) const noexcept {
    return static_cast<float>(stbtt_GetCodepointKernAdvance(&this->info, cp, next))
        * stbtt_ScaleForPixelHeight(&this->info, fontSize);
}

float Font::Advance(uint32_t  cp, float fontSize) const noexcept {
    int32_t advance{};
    int32_t lsb{};

    stbtt_GetCodepointHMetrics(&this->info, cp, &advance, &lsb);

    return advance * stbtt_ScaleForPixelHeight(&this->info, fontSize);
}

bool Font::Exists(uint32_t cp) const noexcept {
    return stbtt_FindGlyphIndex (&this->info, cp) > 0;
}

} // namespace ls
