/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextLine.h"
#include <iterator>

namespace ls {

bool IsBreakingSpace(int32_t codepoint);

TextLine::TextLine(std::shared_ptr<Font> font) : font(font) {
}

TextLine::TextLine(TextLine&& other) {
    this->font = std::move(other.font);
    this->width = other.width;
    this->chars = std::move(other.chars);
}

bool TextLine::Append(int32_t codepoint, float maxWidth) {
    if (!this->font) {
        return false;
    }

    codepoint = this->font->LoadGlyphOrFallback(codepoint);

    if (IsBreakingSpace(codepoint) && (this->chars.empty() || IsBreakingSpace(this->chars.back()))) {
        return true;
    }

    auto newCharWidth{ this->font->GetGlyphAdvance() };

    if (!this->chars.empty()) {
        newCharWidth += this->font->GetKerning(this->chars.back(), codepoint);
    }

    if (maxWidth > 0 && (this->width + newCharWidth) > maxWidth) {
        return false;
    }

    this->width += newCharWidth;
    this->chars.push_back(codepoint);

    return true;
}

void TextLine::Paint(float x, float y, const Surface& target) {
    static const auto SurfaceId = 0;
    static const auto BaselineOffsetId = 1;

    if (!this->font) {
        return;
    }

    int32_t previousCodepoint{0};
    auto position{x};
    auto ascent{ this->font->GetAscent() };

    for (auto codepoint : this->chars) {
        codepoint = this->font->LoadGlyph(codepoint);

        if (this->font->HasGlyphBitmap()) {
            auto result{
                this->font->GetGlyphBitmapSubpixel(position - floor(position), ascent - floor(ascent))
            };

            target.Blit(
                static_cast<int32_t>(position),
                static_cast<int32_t>(ceil(y + ascent)) + std::get<BaselineOffsetId>(result),
                std::get<SurfaceId>(result));
        }

        if (previousCodepoint) {
            position += this->font->GetKerning(previousCodepoint, codepoint);
        }

        position += this->font->GetGlyphAdvance();

        previousCodepoint = codepoint;
    }
}

TextLine TextLine::Break(bool hardLineBreak) {
    this->Finalize();

    if (hardLineBreak) {
        return { this->font };
    }

    auto lastSpaceIndex{this->chars.size()};

    while (lastSpaceIndex--) {
        if (IsBreakingSpace(this->chars[lastSpaceIndex])) {
            break;
        }
    }

    if (lastSpaceIndex < 0) {
        return { this->font };
    }

    TextLine nextLine{ this->font };
    auto nextLineStart{ this->chars.begin() + (lastSpaceIndex + 1) };

    if (nextLineStart != this->chars.end()) {
        std::copy(
            nextLineStart,
            this->chars.end(),
            std::back_insert_iterator<decltype(nextLine.chars)>(nextLine.chars));

        nextLine.width = nextLine.CalculateWidth();

        this->chars.erase(nextLineStart, this->chars.end());
        this->width = this->CalculateWidth();
    }

    return nextLine;
}

void TextLine::Finalize() {
    if (!this->chars.empty() && IsBreakingSpace(this->chars.back())) {
        this->chars.pop_back();
        this->width = this->CalculateWidth();
    }
}

void TextLine::Ellipsize(float maxWidth) {
    if (this->chars.empty() || !this->font->HasEllipsisCodepoint()) {
        return;
    }

    int32_t ellipsisCodepoint;
    int32_t ellipsisCodepointRepeat;

    std::tie(ellipsisCodepoint, ellipsisCodepointRepeat) = this->font->GetEllipsisCodepoint();

    this->font->LoadGlyph(ellipsisCodepoint);

    auto ellipsisLength{ this->font->GetGlyphAdvance() * ellipsisCodepointRepeat };

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
        this->font->LoadGlyph(last);
        this->width -= this->font->GetGlyphAdvance();

        if (!this->chars.empty()) {
            this->width -= this->font->GetKerning(this->chars.back(), last);
        }
    }
}

float TextLine::CalculateWidth() const {
    auto result{ 0.f };
    auto previousCodepoint{0};

    for (auto codepoint : this->chars) {
        codepoint = this->font->LoadGlyph(codepoint);

        if (previousCodepoint) {
            result += this->font->GetKerning(previousCodepoint, codepoint);
        }

        result += this->font->GetGlyphAdvance();

        previousCodepoint = codepoint;
    }

    return result;
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

} // namespace ls
