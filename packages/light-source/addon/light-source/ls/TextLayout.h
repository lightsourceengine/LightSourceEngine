/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>
#include <vector>
#include "StyleEnums.h"

namespace ls {

class Font;
class TextSceneNode;

struct TextLine {
    float width{0};
    std::vector<uint32_t> chars;
};

struct TextLayoutFont {
    Font* font;
    float fontSize;
    float lineHeight;
    uint32_t fallbackCodepoint;

    TextLayoutFont(Font* font, float fontSize, float lineHeight) noexcept;

    float Advance(uint32_t codepoint) const noexcept;
    float Kerning(uint32_t codepoint, uint32_t next) const noexcept;
    bool Exists(uint32_t codepoint) const noexcept;
};

class TextLayout {
 public:
    void Layout();
    void Layout(const TextLayoutFont& font, StyleTextOverflow textOverflow, int32_t maxLines,
                const std::string& text, float width, float height);

    bool IsEmpty() const noexcept { return this->lines.empty(); }
    float Width() const noexcept { return this->computedWidth; }
    float Height() const noexcept { return this->computedHeight; }

 private:
    std::vector<TextLine> lines;
    float computedWidth{};
    float computedHeight{};

    friend TextSceneNode;
};

} // namespace ls
