/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Font.h"
#include "Timer.h"
#include <fmt/format.h>

namespace ls {

constexpr int32_t UnicodeFallback{ 0xFFFD };
constexpr int32_t UnicodeQuestionMark{ 0x3F };
constexpr int32_t UnicodeDot{ 0x2E };
constexpr int32_t UnicodeEllipsis{ 0x2026 };

Font::Font(std::shared_ptr<stbtt_fontinfo> info, int32_t fontSize)
: info(info), bitmapBuffer(fontSize*fontSize), fontSize(fontSize) {
    auto fontInfo{ info.get() };
    int32_t ascent{0};
    int32_t descent{0};
    int32_t lineGap{0};

    stbtt_GetFontVMetrics(fontInfo, &ascent, &descent, &lineGap);

    this->scale = stbtt_ScaleForPixelHeight(fontInfo, static_cast<float>(fontSize));
    this->ascent = ascent * scale;
    this->lineHeight = (ascent - descent + lineGap) * this->scale;

    if (this->CodepointToGlyphIndex(UnicodeFallback) > 0) {
        this->fallbackCodepoint = UnicodeFallback;
    } else if (this->CodepointToGlyphIndex(UnicodeQuestionMark) > 0) {
        this->fallbackCodepoint = UnicodeQuestionMark;
    } else {
        this->fallbackCodepoint = 0;
    }

    if (this->CodepointToGlyphIndex(UnicodeEllipsis) > 0) {
        this->ellipsisCodepoint = UnicodeEllipsis;
        this->ellipsisCodepointRepeat = 1;
    } else if (this->CodepointToGlyphIndex(UnicodeDot) > 0) {
        this->ellipsisCodepoint = UnicodeDot;
        this->ellipsisCodepointRepeat = 3;
    }
}

Font::~Font() {
    if (this->info) {
        for (auto& it : this->shapeTable) {
            if (it.second.vertices) {
                stbtt_FreeShape(info.get(), it.second.vertices);
            }
        }
    }
}

bool Font::HasEllipsisCodepoint() const {
    return this->ellipsisCodepoint > 0;
}

std::tuple<int32_t, int32_t> Font::GetEllipsisCodepoint() const {
    return std::make_tuple(this->ellipsisCodepoint, this->ellipsisCodepointRepeat);
}

int32_t Font::LoadGlyph(int32_t codepoint) {
    this->codepoint = codepoint;
    this->glyphIndex = this->CodepointToGlyphIndex(codepoint);

    if (this->glyphIndex <= 0) {
        this->codepoint = 0;
        this->glyphIndex = 0;
    }

    return this->codepoint;
}

int32_t Font::LoadGlyphOrFallback(int32_t codepoint) {
    auto result{ this->LoadGlyph(codepoint) };

    if (result <= 0) {
        return this->LoadGlyph(this->fallbackCodepoint);
    }

    return result;
}

float Font::GetGlyphAdvance() const {
    int32_t advance{0};

    stbtt_GetGlyphHMetrics(this->info.get(), this->glyphIndex, &advance, nullptr);

    return advance * this->scale;
}

float Font::GetGlyphKerning(int32_t nextCodepoint) const {
    auto key{ (static_cast<uint64_t>(codepoint) << 32) | (static_cast<uint64_t>(nextCodepoint) & 0xFFFFFFFF) };
    auto it{ this->kerningTable.find(key) };

    if (it != this->kerningTable.end()) {
        return it->second;
    }

    auto kerning{ stbtt_GetGlyphKernAdvance(
        this->info.get(),
        this->glyphIndex,
        this->CodepointToGlyphIndex(nextCodepoint)) * this->scale
    };

    this->kerningTable.insert(std::make_pair(key, kerning));

    return kerning;
}

float Font::GetKerning(int32_t codepoint, int32_t nextCodepoint) const {
    auto key{ (static_cast<uint64_t>(codepoint) << 32) | (static_cast<uint64_t>(nextCodepoint) & 0xFFFFFFFF) };
    auto it{ this->kerningTable.find(key) };

    if (it != this->kerningTable.end()) {
        return it->second;
    }

    auto kerning{ stbtt_GetGlyphKernAdvance(
        this->info.get(),
        this->CodepointToGlyphIndex(codepoint),
        this->CodepointToGlyphIndex(nextCodepoint)) * this->scale
    };

    this->kerningTable.insert(std::make_pair(key, kerning));

    return kerning;
}

bool Font::HasGlyphBitmap() const {
    return stbtt_IsGlyphEmpty(this->info.get(), this->glyphIndex) == 0;
}

std::tuple<Surface, int32_t> Font::GetGlyphBitmapSubpixel(float shiftX, float shiftY) const {
    if (this->glyphIndex <= 0) {
        return std::make_tuple(Surface(nullptr, 0, 0), 0);
    }

    int32_t bitmapWidth{0};
    int32_t bitmapHeight{0};
    int32_t bitmapX{0};
    int32_t baselineOffset{0};

    auto bitmap{ this->GetGlyphBitmapSubpixel(
        shiftX,
        shiftY,
        this->glyphIndex,
        &bitmapWidth,
        &bitmapHeight,
        &bitmapX,
        &baselineOffset)
    };

    if (!bitmap) {
        bitmapWidth = bitmapHeight = 0;
    }

    return std::make_tuple(Surface(bitmap, bitmapWidth, bitmapHeight), baselineOffset);
}

int32_t Font::CodepointToGlyphIndex(int32_t codepoint) const {
    auto it{ this->glyphIndexTable.find(codepoint) };

    if (it != this->glyphIndexTable.end()) {
        return it->second;
    }

    auto glyphIndex{ stbtt_FindGlyphIndex(this->info.get(), codepoint) };

    this->glyphIndexTable.insert(std::make_pair(codepoint, glyphIndex));

    return glyphIndex;
}

Font::Shape Font::GetGlyphShape(int32_t glyphIndex) const {
    auto it{ this->shapeTable.find(glyphIndex) };

    if (it != this->shapeTable.end()) {
        return it->second;
    }

    stbtt_vertex* vertices{ nullptr };
    int32_t numVertices{ stbtt_GetGlyphShape(this->info.get(), glyphIndex, &vertices) };
    Shape shape{ vertices, numVertices };

    this->shapeTable.insert(std::make_pair(glyphIndex, shape));

    return shape;
}

stbtt__bitmap Font::ConstructBitmap(int32_t width, int32_t height) const {
    if (width <= 0 || height <= 0) {
        return { 0, 0, 0, nullptr };
    }

    auto capacity{ static_cast<int32_t>(this->bitmapBuffer.capacity()) };

    if (capacity < width * height) {
        try {
            this->bitmapBuffer.reserve(width * height);
        } catch (std::exception& e) {
            fmt::println("bitmapBuffer allocation error: {}", e.what());
            return { 0, 0, 0, nullptr };
        }
    }

    return { width, height, width, this->bitmapBuffer.data() };
}

std::shared_ptr<uint8_t> Font::GetGlyphBitmapSubpixel(float shiftX, float shiftY, int32_t glyphIndex,
        int32_t* width, int32_t* height, int32_t* xOffset, int32_t* yOffset) const {
    *width = *height = *xOffset = *yOffset = 0;

    int32_t ix0;
    int32_t iy0;
    int32_t ix1;
    int32_t iy1;

    stbtt_GetGlyphBitmapBoxSubpixel(
        this->info.get(),
        glyphIndex, this->scale,
        this->scale,
        shiftX,
        shiftY,
        &ix0,
        &iy0,
        &ix1,
        &iy1);

    auto gbm{ this->ConstructBitmap(ix1 - ix0, iy1 - iy0) };

    if (gbm.w <= 0 || gbm.h <= 0) {
        return {};
    }

    auto shape{ this->GetGlyphShape(glyphIndex) };

    if (shape.IsEmpty()) {
        return {};
    }

    stbtt_Rasterize(
        &gbm,
        0.10f,
        shape.vertices,
        shape.numVertices,
        this->scale,
        this->scale,
        shiftX,
        shiftY,
        ix0,
        iy0,
        1,
        this->info->userdata);

    *width = gbm.w;
    *height = gbm.h;
    *xOffset = ix0;
    *yOffset = iy0;

    return std::shared_ptr<uint8_t>(gbm.pixels, [](uint8_t* data){});
}

} // namespace ls
