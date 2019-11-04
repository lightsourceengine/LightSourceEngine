/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextLayout.h"
#include "Font.h"
#include <cmath>
#include <algorithm>
#include <utf8.h>

namespace ls {

static bool TextLineAppend(TextLine& line, uint32_t codepoint, const TextLayoutFont& font, float maxWidth);
static TextLine TextLineBreak(TextLine& line, const TextLayoutFont& font, bool hardLineBreak);
static void TextLineFinalize(TextLine& line, const TextLayoutFont& font);
static void TextLineEllipsize(TextLine& line, const TextLayoutFont& font, float maxWidth);
static float TextLineComputeWidth(TextLine& line, const TextLayoutFont& font);
static bool CanAdvanceLine(int32_t currentLineNumber, float lineHeight, float heightLimit, int32_t maxLines);

constexpr bool IsBreakingSpace(uint32_t codepoint) noexcept {
    switch (codepoint) {
        case 0x0020:
        case 0x180E:
        case 0x2000:
        case 0x2001:
        case 0x2002:
        case 0x2003:
        case 0x2004:
        case 0x2005:
        case 0x2006:
        case 0x2007:
        case 0x2008:
        case 0x2009:
        case 0x200A:
        case 0x200B:
        case 0x205F:
        case 0x3000:
            return true;
        default:
            return false;
    }
}

constexpr uint32_t UnicodeNewLine{ 0x0A };
constexpr uint32_t UnicodeFallback{ 0xFFFD };
constexpr uint32_t UnicodeQuestionMark{ 0x3F };
constexpr uint32_t UnicodeDot{ 0x2E };
constexpr uint32_t UnicodeEllipsis{ 0x2026 };

void TextLayout::Layout() {
    this->lines.clear();
    this->computedWidth = 0;
    this->computedHeight = 0;
}

void TextLayout::Layout(const TextLayoutFont& font, StyleTextOverflow textOverflow, int32_t maxLines,
        const std::string& text, float width, float height) {
    this->Layout();

    if (text.empty() || font.fontSize <= 0 || font.lineHeight <= 0) {
        return;
    }

    auto textIter{ text.begin() };
    uint32_t codepoint;
    bool appendResult;
    bool hardLineBreak;

    this->lines.emplace_back();

    while (textIter != text.end()) {
        codepoint = utf8::unchecked::next(textIter);

        if (codepoint == UnicodeNewLine) {
            appendResult = false;
            hardLineBreak = true;
        } else {
            if (!font.Exists(codepoint)) {
                if (font.fallbackCodepoint) {
                    codepoint = font.fallbackCodepoint;
                } else {
                    codepoint = 0;
                }
            }

            if (codepoint) {
                appendResult = TextLineAppend(this->lines.back(), codepoint, font, width);
            } else {
                appendResult = true;
            }

            hardLineBreak = false;
        }

        if (!appendResult) {
            if (!CanAdvanceLine(static_cast<int32_t>(this->lines.size()), font.lineHeight, height, maxLines)) {
                if (width > 0 && textOverflow == StyleTextOverflowEllipsis) {
                    TextLineEllipsize(this->lines.back(), font, width);
                }

                break;
            }

            this->lines.emplace_back(TextLineBreak(this->lines.back(), font, hardLineBreak));

            if (codepoint != UnicodeNewLine) {
                TextLineAppend(this->lines.back(), codepoint, font, width);
            }
        }
    }

    TextLineFinalize(this->lines.back(), font);

    while (!this->lines.empty()) {
        if (!this->lines.back().chars.empty()) {
            break;
        }

        this->lines.pop_back();
    }

    auto blockWidth{0.f};

    for (auto& textLine : this->lines) {
        blockWidth = std::max(blockWidth, textLine.width);
    }

    this->computedWidth = std::ceil(blockWidth);
    this->computedHeight = std::ceil(this->lines.size() * font.lineHeight);
}

static bool CanAdvanceLine(int32_t currentLineNumber, float lineHeight, float heightLimit, int32_t maxLines) {
    return (maxLines == 0 || currentLineNumber < maxLines)
        && (heightLimit == 0 || (static_cast<float>(currentLineNumber + 1) * lineHeight) < heightLimit);
}

static bool TextLineAppend(TextLine& line, uint32_t codepoint, const TextLayoutFont& font, float maxWidth) {
    if (IsBreakingSpace(codepoint) && (line.chars.empty() || IsBreakingSpace(line.chars.back()))) {
        return true;
    }

    auto newCharWidth{ font.Advance(codepoint) };

    if (!line.chars.empty()) {
        newCharWidth += font.Kerning(line.chars.back(), codepoint);
    }

    if (maxWidth > 0 && (line.width + newCharWidth) > maxWidth) {
        return false;
    }

    line.width += newCharWidth;
    line.chars.push_back(codepoint);

    return true;
}

static TextLine TextLineBreak(TextLine& line, const TextLayoutFont& font, bool hardLineBreak) {
    if (hardLineBreak) {
        TextLineFinalize(line, font);
        return {};
    }

    auto lastSpaceIndex{ static_cast<int32_t>(line.chars.size()) };

    while (lastSpaceIndex--) {
        if (IsBreakingSpace(line.chars[lastSpaceIndex])) {
            break;
        }
    }

    if (lastSpaceIndex < 0) {
        return {};
    }

    TextLine nextLine{};
    auto nextLineStart{ line.chars.begin() + (lastSpaceIndex + 1) };

    if (nextLineStart != line.chars.end()) {
        std::copy(
            nextLineStart,
            line.chars.end(),
            std::back_insert_iterator<decltype(nextLine.chars)>(nextLine.chars));

        nextLine.width = TextLineComputeWidth(nextLine, font);
        line.chars.erase(nextLineStart, line.chars.end());
    }

    TextLineFinalize(line, font);

    return nextLine;
}

static void TextLineFinalize(TextLine& line, const TextLayoutFont& font) {
    if (!line.chars.empty() && IsBreakingSpace(line.chars.back())) {
        line.chars.pop_back();
        line.width = TextLineComputeWidth(line, font);
    }
}

static void TextLineEllipsize(TextLine& line, const TextLayoutFont& font, float maxWidth) {
    if (line.chars.empty()) {
        return;
    }

    uint32_t ellipsisCodepoint;
    int32_t ellipsisRepeat;

    if (font.Exists(UnicodeEllipsis)) {
        ellipsisCodepoint = UnicodeEllipsis;
        ellipsisRepeat = 1;
    } else if (font->Exists(UnicodeDot)) {
        ellipsisCodepoint = UnicodeDot;
        ellipsisRepeat = 3;
    } else {
        return;
    }

    auto ellipsisLength{ font.Advance(ellipsisCodepoint) * ellipsisRepeat };

    while (!line.chars.empty()) {
        if (maxWidth - line.width >= ellipsisLength && !IsBreakingSpace(line.chars.back())) {
            for (auto i{0}; i < ellipsisRepeat; i++) {
                line.chars.push_back(ellipsisCodepoint);
            }

            line.width += ellipsisLength;

            break;
        }

        auto last{ line.chars.back() };

        line.chars.pop_back();
        line.width -= font.Advance(last);

        if (!line.chars.empty()) {
            line.width -= font.Kerning(line.chars.back(), last);
        }
    }
}

static float TextLineComputeWidth(TextLine& line, const TextLayoutFont& font) {
    auto result{0.f};
    auto previousCodepoint{0};

    for (auto codepoint : line.chars) {
        if (previousCodepoint) {
            result += font.Kerning(previousCodepoint, codepoint);
        }

        result += font.Advance(codepoint);

        previousCodepoint = codepoint;
    }

    return result;
}

TextLayoutFont::TextLayoutFont(Font* font, float fontSize, float lineHeight) noexcept
: font(font), fontSize(fontSize), lineHeight(lineHeight) {
    if (font->Exists(UnicodeFallback)) {
        this->fallbackCodepoint = UnicodeFallback;
    } else if (font->Exists(UnicodeQuestionMark)) {
        this->fallbackCodepoint = UnicodeQuestionMark;
    } else {
        this->fallbackCodepoint = 0;
    }
}

float TextLayoutFont::Advance(uint32_t codepoint) const noexcept {
    return this->font->Advance(codepoint, this->fontSize);
}

float TextLayoutFont::Kerning(uint32_t codepoint, uint32_t next) const noexcept {
    return this->font->Kerning(codepoint, next, this->fontSize);
}

bool TextLayoutFont::Exists(uint32_t codepoint) const noexcept {
    return this->font->Exists(codepoint);
}

} // namespace ls
