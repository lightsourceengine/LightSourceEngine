/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>
#include <vector>
#include <tuple>
#include <stb_truetype.h>
#include <ls/Surface.h>

namespace ls {

class Font {
 private:
    struct Shape {
        stbtt_vertex* vertices{};
        int32_t numVertices{};

        Shape() noexcept = default;
        Shape(stbtt_vertex* vertices, const int32_t numVertices) noexcept
            : vertices(vertices), numVertices(numVertices) {}

        bool IsEmpty() const noexcept { return vertices == nullptr || numVertices <= 0; }
    };

 public:
    Font(std::shared_ptr<stbtt_fontinfo>& fontInfo, const int32_t fontSize);
    ~Font();

    float GetAscent() const { return this->ascent; }
    float GetLineHeight() const { return this->lineHeight; }
    int32_t GetSize() const { return this->fontSize; }

    int32_t LoadGlyph(int32_t codepoint);
    int32_t LoadGlyphOrFallback(int32_t codepoint);

    float GetGlyphAdvance() const;
    float GetKerning(int32_t codepoint, int32_t nextCodepoint) const;
    float GetGlyphKerning(int32_t nextCodepoint) const;
    bool HasEllipsisCodepoint() const;
    std::tuple<int32_t, int32_t> GetEllipsisCodepoint() const;
    bool HasGlyphBitmap() const;
    std::tuple<Surface, int32_t> GetGlyphBitmapSubpixel(float shiftX, float shiftY) const;

 private:
    int32_t CodepointToGlyphIndex(int32_t codepoint) const;
    std::shared_ptr<uint8_t> GetGlyphBitmapSubpixel(float shiftX, float shiftY, int32_t glyphIndex,
        int32_t* width, int32_t* height, int32_t* xOffset, int32_t* yOffset) const;
    Shape GetGlyphShape(int32_t glyphIndex) const;
    stbtt__bitmap ConstructBitmap(int32_t width, int32_t height) const;

 private:
    std::shared_ptr<stbtt_fontinfo> info;
    mutable std::vector<uint8_t> bitmapBuffer;
    int32_t fontSize;
    float scale;
    float ascent;
    float lineHeight;
    int32_t glyphIndex{0};
    int32_t codepoint{0};
    int32_t fallbackCodepoint{0};
    int32_t ellipsisCodepoint{0};
    int32_t ellipsisCodepointRepeat{0};
    mutable std::unordered_map<int32_t, int32_t> glyphIndexTable;
    mutable std::unordered_map<int32_t, Shape> shapeTable;
    mutable std::unordered_map<uint64_t, float> kerningTable;
};

} // namespace ls
