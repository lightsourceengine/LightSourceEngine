/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "StyleEnums.h"

namespace ls {

class Font;
class Surface;

namespace internal {

class TextLine {
 public:
    TextLine() = default;
    TextLine(const TextLine& other) = delete;
    TextLine(TextLine&& other);

    bool Append(Font* font, int32_t codepoint, float maxWidth);
    void Paint(Font* font, float x, float y, const Surface& target);
    float Width() const { return this->width; }

    TextLine Break(Font* font, bool hardLineBreak);
    void Finalize(Font* font);
    void Ellipsize(Font* font, float maxWidth);
    bool IsEmpty() const { return this->chars.empty(); }
 private:
    float CalculateWidth(Font* font) const;

 private:
     float width{0};
     std::vector<int32_t> chars;
};

} // namespace internal

class TextBlock {
 public:
    void Layout(const std::string& text, float width, float height,
        bool useEllipsis, float lineHeight, int32_t maxLines);
    void Paint(const Surface& target, float lineHeight, StyleTextAlign textAlign, float alignWidth);
    void SetFont(std::shared_ptr<Font> font) { this->font = font; }
    float GetComputedWidth() const { return this->computedWidth; }
    float GetComputedHeight() const { return this->computedHeight; }

    void Clear() { this->SetComputedSize(0.f, 0.f); this->textLines.clear(); }
    bool IsEmpty() const { return this->textLines.empty(); }

 private:
    void SetComputedSize(float width, float height) { this->computedWidth = width; this->computedHeight = height; }

 private:
    std::vector<internal::TextLine> textLines;
    std::shared_ptr<Font> font;
    float computedWidth{0.f};
    float computedHeight{0.f};
};

} // namespace ls
