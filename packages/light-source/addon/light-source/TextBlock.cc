/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TextBlock.h"
#include "FontSampleResource.h"
#include "Yoga.h"
#include <utf8.h>
#include <algorithm>
#include <fmt/format.h>

namespace ls {

constexpr int32_t UnicodeDot{ 0x2E };
constexpr int32_t UnicodeNewLine{ 0x0A };
constexpr int32_t UnicodeEllipsis{ 0x2026 };

bool IsNonBreakingSpace(int32_t codepoint);

class LayoutLine {
 public:
    LayoutLine(const TextBlock& textBlock, float widthLimit, float heightLimit)
        : textBlock(textBlock), widthLimit(widthLimit), heightLimit(heightLimit),
            fontMetrics(textBlock.font->GetFontMetrics().get()) {
        CodepointMetrics ellipsisMetrics;

        if (fontMetrics->GetCodepointMetrics(UnicodeEllipsis, &ellipsisMetrics)) {
            this->ellipsisRepeat = 1;
            this->ellipsisLength = ellipsisMetrics.xAdvance;
            this->ellipsisCodepoint = UnicodeEllipsis;
        } else if (fontMetrics->GetCodepointMetrics(UnicodeDot, &ellipsisMetrics)) {
            this->ellipsisRepeat = 3;
            this->ellipsisLength = ellipsisMetrics.xAdvance * this->ellipsisRepeat;
            this->ellipsisCodepoint = UnicodeDot;
        }
    }

    void Align() const {
        auto align{ textBlock.textAlign };

        if (this->widthLimit == 0 || align == StyleTextAlignLeft) {
            return;
        }

        auto adjustX{ align == StyleTextAlignCenter ? (this->widthLimit - this->x) / 2.f : this->widthLimit - this->x };

        for (auto i{ this->lineStartIndex }; i <= (this->lineStartIndex + this->cursorIndex); i++) {
            this->textBlock.quads[i].x += adjustX;
        }
    }

    bool LineBreak() {
        if (this->CanAdvanceY()) {
            this->DoLineBreak();

            return true;
        } else {
            this->Ellipsize();
            return false;
        }
    }

    bool LineBreakAtLastSpace() {
        if (this->breakingSpaceIndex == -1) {
            return LineBreak();
        }

        if (!this->CanAdvanceY()) {
            this->Ellipsize();
            return false;
        }

        auto spaceIndex{ this->lineStartIndex + this->breakingSpaceIndex };
        std::vector<int32_t> codepoints;
        auto quadsLen{ static_cast<int32_t>(this->textBlock.quads.size()) };
        CodepointMetrics metrics;

        for (auto i{ spaceIndex + 1 }; i < quadsLen; i++) {
            auto cp{ this->textBlock.quads[i].codepoint };

            codepoints.push_back(cp);

            this->fontMetrics->GetCodepointMetrics(cp, &metrics);
            this->x -= metrics.xAdvance;
        }

        this->textBlock.quads.erase(this->textBlock.quads.begin() + spaceIndex, this->textBlock.quads.end());
        this->DoLineBreak();

        auto codepointsLen{ static_cast<int32_t>(codepoints.size()) };

        for (auto i{ 0 }; i < codepointsLen; i++) {
            this->codepoint = codepoints[i];
            this->fontMetrics->GetCodepointMetrics(this->codepoint, &metrics);
            this->DoAppend(codepoints[i], (i + 1 < codepointsLen) ? codepoints[i + 1] : 0, metrics);
        }

        return true;
    }

    bool CanAppend(const CodepointMetrics& metrics) {
        return this->widthLimit == 0 || (x + metrics.xAdvance) <= this->widthLimit;
    }

    bool Append(int32_t codepoint, int32_t nextCodepoint, const CodepointMetrics& metrics) {
        if (!this->CanAppend(metrics)) {
            return this->LineBreakAtLastSpace();
        }

        auto space{ IsNonBreakingSpace(codepoint) };

        if (this->cursorIndex < 0 && space) {
            return true;
        }

        this->DoAppend(codepoint, nextCodepoint, metrics);

        if (space) {
            this->breakingSpaceIndex = this->cursorIndex;
        }

        return true;
    }

    void Finish(float* computedWidth, float* computedHeight) {
        if (x > 0) {
            this->Align();
            maxWidth = std::max(x, maxWidth);
        }

        *computedWidth = maxWidth;
        *computedHeight = this->GetHeight();
    }

    int32_t GetCodepoint() const { return codepoint; }

 private:
    float GetHeight() const {
        return (this->lineNumber + 1) * fontMetrics->GetLineHeight();
    }

    bool CanAdvanceY() const {
        auto maxLines{ this->textBlock.maxLines };

        return (maxLines == 0 || this->lineNumber + 1 < maxLines) && (this->heightLimit == 0
            || (this->fontMetrics->GetLineHeight() * (this->lineNumber + 2)) <= this->heightLimit);
    }

    bool CanEllipsize() const {
        return this->ellipsisCodepoint > 0 && this->textBlock.textOverflow == StyleTextOverflowEllipsis
            && this->widthLimit > 0;
    }

    void Ellipsize() {
        if (!this->CanEllipsize() || ellipsisStartIndex == -1) {
            return;
        }

        CodepointMetrics metrics;
        auto quadsLen{ static_cast<int32_t>(this->textBlock.quads.size()) };

        for (auto i{ this->lineStartIndex + this->ellipsisStartIndex + 1 }; i < quadsLen; i++) {
            this->fontMetrics->GetCodepointMetrics(this->textBlock.quads[i].codepoint, &metrics);
            this->x -= metrics.xAdvance;
        }

        this->textBlock.quads.erase(
            this->textBlock.quads.begin() + this->lineStartIndex + this->ellipsisStartIndex + 1,
            this->textBlock.quads.end());

        this->fontMetrics->GetCodepointMetrics(this->ellipsisCodepoint, &metrics);

        for (auto i{ 0 }; i < this->ellipsisRepeat; i++) {
            this->DoAppend(this->ellipsisCodepoint, -1, metrics);
        }
    }

    void DoLineBreak() {
        this->Align();

        this->maxWidth = std::max(this->x, this->maxWidth);
        this->cursorIndex = -1;
        this->breakingSpaceIndex = -1;
        this->ellipsisStartIndex = -1;
        this->lineStartIndex = static_cast<int32_t>(this->textBlock.quads.size());
        this->lineNumber++;
        this->codepoint = 0;
    }

    void DoAppend(int32_t codepoint, int32_t nextCodepoint, const CodepointMetrics& metrics) {
        this->cursorIndex++;

        float y{ this->fontMetrics->GetLineHeight() * this->lineNumber };

        this->textBlock.quads.push_back({
            this->x - metrics.xOffset,
            y - metrics.yOffset + (this->fontMetrics->GetAscent() - metrics.height),
            metrics.width,
            metrics.height,
            codepoint
        });

        this->codepoint = codepoint;
        this->x += metrics.xAdvance;
        this->x += (nextCodepoint > 0) ? this->fontMetrics->GetKerning(codepoint, nextCodepoint) : 0;

        if (this->CanEllipsize() && (this->x + this->ellipsisLength) <= widthLimit) {
            this->ellipsisStartIndex = this->cursorIndex;
        }
    }

 private:
    const TextBlock& textBlock;
    float widthLimit;
    float heightLimit;
    FontMetrics* fontMetrics{};

    int32_t lineStartIndex{0};

    int32_t codepoint{0};
    int32_t lineNumber{ 0 };

    int32_t cursorIndex{ -1 };
    int32_t breakingSpaceIndex{ -1 };
    int32_t ellipsisStartIndex{ -1 };
    float x{0};
    float maxWidth{0};
    int32_t ellipsisCodepoint{0};
    int32_t ellipsisRepeat{0};
    float ellipsisLength{0};
};

TextBlock::TextBlock() {
}

TextBlock::~TextBlock() {
    this->SetFont(nullptr);
}

void TextBlock::SetFont(FontSampleResource* font) {
    if (font == this->font) {
        return;
    }

    if (this->font) {
        this->font->RemoveRef();
        this->font = nullptr;
    }

    if (font) {
        font->AddRef();
        this->font = font;
    }

    this->MarkDirty();
}

void TextBlock::SetText(const std::string& text) {
    if (this->text != text) {
        this->text = text;
        this->MarkDirty();
    }
}

void TextBlock::SetText(std::string&& text) {
    if (this->text != text) {
        this->text = std::move(text);
        this->MarkDirty();
    }
}

void TextBlock::SetTextOverflow(StyleTextOverflow textOverflow) {
    if (this->textOverflow != textOverflow) {
        this->textOverflow = textOverflow;
        this->MarkDirty();
    }
}

void TextBlock::SetTextAlign(StyleTextAlign textAlign) {
    if (this->textAlign != textAlign) {
        this->textAlign = textAlign;
        this->MarkDirty();
    }
}

void TextBlock::SetMaxLines(int32_t maxLines) {
    if (this->maxLines != maxLines) {
        this->maxLines = maxLines;
        this->MarkDirty();
    }
}

void TextBlock::Layout(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
    if (!this->font || !this->font->IsReady()) {
        return;
    }

    if (!this->isDirty && this->measuredWidth == width && this->measuredWidthMode == widthMode
            && this->measuredHeight == height && this->measuredHeightMode == heightMode) {
        return;
    }

    this->quads.clear();

    CodepointMetrics metrics;
    auto textIter{ this->text.begin() };
    auto fontMetrics{ this->font->GetFontMetrics() };
    LayoutLine line(*this, width, height);

    while (textIter != this->text.end()) {
        auto codepoint{ utf8::unchecked::next(textIter) };

        if (codepoint == UnicodeNewLine) {
            if (line.LineBreak()) {
                continue;
            } else {
                break;
            }
        }

        fontMetrics->GetCodepointMetricsOrFallback(codepoint, &metrics);

        if (IsNonBreakingSpace(codepoint) && IsNonBreakingSpace(line.GetCodepoint())) {
            continue;
        }

        if (!line.Append(codepoint, (textIter != text.end()) ? utf8::unchecked::peek_next(textIter) : -1, metrics)) {
            break;
        }
    }

    line.Finish(&this->computedWidth, &this->computedHeight);

    this->measuredWidth = width;
    this->measuredWidthMode = widthMode;
    this->measuredHeight = height;
    this->measuredHeightMode = heightMode;
    this->isDirty = false;
}

void TextBlock::Paint(Renderer* renderer, float x, float y, int64_t color) {
    for (auto& quad : this->quads) {
        if (quad.width != 0) {
            renderer->DrawFillRect({
                YGRoundValueToPixelGrid(x + quad.x, 1.f, false, false),
                YGRoundValueToPixelGrid(y + quad.y, 1.f, false, false),
                YGRoundValueToPixelGrid(quad.width, 1.f, false, false),
                YGRoundValueToPixelGrid(quad.height, 1.f, false, false)
            }, color);
        }
    }
}

void TextBlock::MarkDirty() {
    this->isDirty = true;
    this->computedWidth = this->computedHeight = 0;
}

inline
bool IsNonBreakingSpace(int32_t codepoint) {
    switch(codepoint) {
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
