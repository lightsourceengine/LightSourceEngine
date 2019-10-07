/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextBlock.h"
#include "Font.h"
#include <ls/Surface.h>
#include <cmath>
#include <algorithm>
#include <utf8.h>

namespace ls {

bool IsBreakingSpace(int32_t codepoint);
bool CanAdvanceLine(int32_t currentLineNumber, float lineHeight, float heightLimit, int32_t maxLines);

constexpr int32_t UnicodeNewLine{ 0x0A };

void TextBlock::Layout(const std::string& text, float width, float height,
        bool useEllipsis, float lineHeight, int32_t maxLines) {
    this->textLines.clear();

    if (!this->font || text.empty() || lineHeight <= 0) {
        this->SetComputedSize(0, 0);
        return;
    }

    auto textIter{ text.begin() };
    int32_t codepoint;
    bool appendResult;
    bool hardLineBreak;
    auto fontPtr{ this->font.get() };

    this->textLines.emplace_back();

    while (textIter != text.end()) {
        codepoint = utf8::unchecked::next(textIter);

        if (codepoint == UnicodeNewLine) {
            appendResult = false;
            hardLineBreak = true;
        } else {
            appendResult = this->textLines.back().Append(fontPtr, codepoint, width);
            hardLineBreak = false;
        }

        if (!appendResult) {
            if (!CanAdvanceLine(static_cast<int32_t>(this->textLines.size()), lineHeight, height, maxLines)) {
                if (width > 0 && useEllipsis) {
                    this->textLines.back().Ellipsize(fontPtr, width);
                }

                break;
            }

            this->textLines.emplace_back(this->textLines.back().Break(fontPtr, hardLineBreak));
        }
    }

    this->textLines.back().Finalize(fontPtr);

    while (!this->textLines.empty()) {
        if (!this->textLines.back().IsEmpty()) {
            break;
        }

        this->textLines.pop_back();
    }

    auto blockWidth{0.f};

    for (auto& textLine : this->textLines) {
        blockWidth = std::max(blockWidth, textLine.Width());
    }

    this->SetComputedSize(std::ceil(blockWidth), std::ceil(this->textLines.size() * lineHeight));
}

void TextBlock::Paint(const Surface& target, float lineHeight, StyleTextAlign textAlign, float alignWidth) {
    if (!this->font) {
        return;
    }

    auto xpos{ 0.f };
    auto ypos{ 0.f };
    auto fontPtr{ this->font.get() };

    for (auto& textLine : this->textLines) {
        switch (textAlign) {
            case StyleTextAlignLeft:
                xpos = 0.f;
                break;
            case StyleTextAlignCenter:
                xpos = (alignWidth - textLine.Width()) / 2.f;
                break;
            case StyleTextAlignRight:
                xpos = alignWidth - textLine.Width();
                break;
        }

        textLine.Paint(fontPtr, xpos, ypos, target);
        ypos += lineHeight;
    }
}

bool CanAdvanceLine(int32_t currentLineNumber, float lineHeight, float heightLimit, int32_t maxLines) {
    return (maxLines == 0 || currentLineNumber < maxLines)
        && (heightLimit == 0 || ((currentLineNumber + 1) * lineHeight) < heightLimit);
}

bool IsBreakingSpace(int32_t codepoint) {
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
    }

    return false;
}

namespace internal {

TextLine::TextLine(TextLine&& other) {
    this->width = other.width;
    this->chars = std::move(other.chars);
}

bool TextLine::Append(Font* font, int32_t codepoint, float maxWidth) {
    codepoint = font->LoadGlyphOrFallback(codepoint);

    if (IsBreakingSpace(codepoint) && (this->chars.empty() || IsBreakingSpace(this->chars.back()))) {
        return true;
    }

    auto newCharWidth{ font->GetGlyphAdvance() };

    if (!this->chars.empty()) {
        newCharWidth += font->GetKerning(this->chars.back(), codepoint);
    }

    if (maxWidth > 0 && (this->width + newCharWidth) > maxWidth) {
        return false;
    }

    this->width += newCharWidth;
    this->chars.push_back(codepoint);

    return true;
}

void TextLine::Paint(Font* font, float x, float y, const Surface& target) {
    static const auto SurfaceId = 0;
    static const auto BaselineOffsetId = 1;

    int32_t previousCodepoint{0};
    auto position{x};
    auto ascent{ font->GetAscent() };

    for (auto codepoint : this->chars) {
        codepoint = font->LoadGlyph(codepoint);

        if (font->HasGlyphBitmap()) {
            auto result{
                font->GetGlyphBitmapSubpixel(position - floor(position), ascent - floor(ascent))
            };

            target.Blit(
                static_cast<int32_t>(position),
                static_cast<int32_t>(ceil(y + ascent)) + std::get<BaselineOffsetId>(result),
                std::get<SurfaceId>(result));
        }

        if (previousCodepoint) {
            position += font->GetKerning(previousCodepoint, codepoint);
        }

        position += font->GetGlyphAdvance();

        previousCodepoint = codepoint;
    }
}

TextLine TextLine::Break(Font* font, bool hardLineBreak) {
    this->Finalize(font);

    if (hardLineBreak) {
        return {};
    }

    auto lastSpaceIndex{ static_cast<int32_t>(this->chars.size()) };

    while (lastSpaceIndex--) {
        if (IsBreakingSpace(this->chars[lastSpaceIndex])) {
            break;
        }
    }

    if (lastSpaceIndex < 0) {
        return {};
    }

    TextLine nextLine;
    auto nextLineStart{ this->chars.begin() + (lastSpaceIndex + 1) };

    if (nextLineStart != this->chars.end()) {
        std::copy(
            nextLineStart,
            this->chars.end(),
            std::back_insert_iterator<decltype(nextLine.chars)>(nextLine.chars));

        nextLine.width = nextLine.CalculateWidth(font);

        this->chars.erase(nextLineStart, this->chars.end());
        this->width = this->CalculateWidth(font);
    }

    return nextLine;
}

void TextLine::Finalize(Font* font) {
    if (!this->chars.empty() && IsBreakingSpace(this->chars.back())) {
        this->chars.pop_back();
        this->width = this->CalculateWidth(font);
    }
}

void TextLine::Ellipsize(Font* font, float maxWidth) {
    if (this->chars.empty() || !font->HasEllipsisCodepoint()) {
        return;
    }

    int32_t ellipsisCodepoint;
    int32_t ellipsisCodepointRepeat;

    std::tie(ellipsisCodepoint, ellipsisCodepointRepeat) = font->GetEllipsisCodepoint();

    font->LoadGlyph(ellipsisCodepoint);

    auto ellipsisLength{ font->GetGlyphAdvance() * ellipsisCodepointRepeat };

    while (!this->chars.empty()) {
        if (maxWidth - width >= ellipsisLength && !IsBreakingSpace(this->chars.back())) {
            for (auto i{0}; i < ellipsisCodepointRepeat; i++) {
                this->chars.push_back(ellipsisCodepoint);
            }

            this->width += ellipsisLength;

            break;
        }

        auto last{ this->chars.back() };

        this->chars.pop_back();
        font->LoadGlyph(last);
        this->width -= font->GetGlyphAdvance();

        if (!this->chars.empty()) {
            this->width -= font->GetKerning(this->chars.back(), last);
        }
    }
}

float TextLine::CalculateWidth(Font* font) const {
    auto result{ 0.f };
    auto previousCodepoint{0};

    for (auto codepoint : this->chars) {
        codepoint = font->LoadGlyph(codepoint);

        if (previousCodepoint) {
            result += font->GetKerning(previousCodepoint, codepoint);
        }

        result += font->GetGlyphAdvance();

        previousCodepoint = codepoint;
    }

    return result;
}

} // namespace internal

} // namespace ls
