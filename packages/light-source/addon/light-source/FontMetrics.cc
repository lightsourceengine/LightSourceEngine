/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FontMetrics.h"
#include <fmt/format.h>

namespace ls {

constexpr int32_t UnicodeSpace{ 0x20 };
constexpr int32_t UnicodeFallback{ 0xFFFD };
constexpr int32_t UnicodeQuestionMark{ 0x3F };

FontMetrics::FontMetrics(std::shared_ptr<stbtt_fontinfo> fontInfo, int32_t fontSize) : fontInfo(fontInfo) {
    auto info{ this->fontInfo.get() };

    int32_t ascent{};
    int32_t descent{};
    int32_t lineGap{};

    stbtt_GetFontVMetrics(info, &ascent, &descent, &lineGap);

    this->scale = stbtt_ScaleForPixelHeight(info, fontSize);
    this->ascent = ascent * scale;
    this->lineHeight = (ascent - descent + lineGap) * scale;

    if (stbtt_FindGlyphIndex(info, UnicodeFallback) > 0) {
        fallbackCodepoint = UnicodeFallback;
    } else if (stbtt_FindGlyphIndex(info, UnicodeQuestionMark) > 0) {
        fallbackCodepoint = UnicodeQuestionMark;
    } else if (stbtt_FindGlyphIndex(info, UnicodeSpace) > 0) {
        fallbackCodepoint = UnicodeSpace;
    } else {
        fallbackCodepoint = 0;
    }
}

bool FontMetrics::GetCodepointMetrics(int32_t codepoint, CodepointMetrics* out) const {
    auto info{ this->fontInfo.get() };
    auto glyphIndex{ stbtt_FindGlyphIndex(info, codepoint) };

    if (glyphIndex <= 0) {
        return false;
    }

    int32_t x0{};
    int32_t y0{};
    int32_t x1{};
    int32_t y1{};
    int32_t xAdvance{};
    int32_t lsb{};

    stbtt_GetGlyphHMetrics(info, glyphIndex, &xAdvance, &lsb);
    stbtt_GetGlyphBox(info, glyphIndex, &x0, &y0, &x1, &y1);

    out->xOffset = x0 * scale;
    out->yOffset = y0 * scale;
    out->width = (x1 - x0) * scale;
    out->height = (y1 - y0) * scale;
    out->xAdvance = xAdvance * scale;

    return true;
}

void FontMetrics::GetCodepointMetricsOrFallback(int32_t codepoint, CodepointMetrics* out) const {
    if (!this->GetCodepointMetrics(codepoint, out)) {
        this->GetCodepointMetrics(fallbackCodepoint, out);
    }
}

float FontMetrics::GetKerning(int32_t codepoint, int32_t nextCodepoint) {
    return stbtt_GetCodepointKernAdvance(this->fontInfo.get(), codepoint, nextCodepoint) * scale;
}

} // namespace ls
