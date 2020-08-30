/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/Renderer.h>
#include <ls/RenderingContext2D.h>
#include <ls/Resources.h>
#include <ls/Style.h>
#include <ls/StyleResolver.h>
#include <ls/TextBlock.h>
#include <ls/Timer.h>
#include <math.h>
#include <utf8.h>
#include <cctype>
#include <utility>

namespace ls {

static std::size_t StringLength(const std::string& utf8) noexcept;
static void ApplyTransform(uint32_t* codepoints, std::size_t size, StyleTextTransform transform) noexcept;

void TextBlock::Shape(const std::string& utf8, FontFace* fontFace, Style* style, const StyleResolver& resolver,
        float maxWidth, float maxHeight) {
    /*
     * Blend2D does not directly expose font metrics per character nor detailed glyph information. The context fill
     * methods for text are limited to text strings and BLGlyphRun (a view on a BLGlyphBuffer). BLGlyphBuffer is
     * effectively immutable, as no characters can be added or removed. BLGlyphBuffer contains either characters OR
     * glyph ids and computed glyph metrics. Once converted to glyph ids, character information is lost. There are
     * no apis for working with glyph ids directly. The limitations make formatting multi-line text blocks a little
     * challenging.
     *
     * I need to map glyph (ids and metrics) back to the original characters in order to measure lines. With that
     * information, I can create BLGlyphRun views for each line referencing the original buffer. The BLGlyphRun
     * views can be rendered to the BLContext.
     *
     * One option is to create two BLGlyphBuffer objects, one containing characters and the other contains glyph
     * information. However, I don't like that this solution uses a lot of memory, but it would be slightly
     * easier to code.
     *
     * The option I chose was to create a BLGlyphBuffer for the glyphs and use the original string for character
     * information. As long as I decode UTF8 characters from the string, the mapping between the string characters
     * and BLGlyphBuffer are 1:1. TextIterator wraps synchronous traversal of glyphs and characters. With that,
     * I am able to measure lines and take BLGlyphRun views of the BLGlyphBuffer for rendering.
     */
    BLResult result;

    this->Invalidate();

    if (utf8.empty() || !fontFace || !style || fontFace->GetState() != Resource::Ready) {
        return;
    }

    const auto fontSize = resolver.ResolveFontSize(style->fontSize);

    if (fontSize <= 0 || style->fontFamily.empty()) {
        return;
    }

    this->font = fontFace->GetFont(fontSize);

    if (this->font.empty()) {
        return;
    }

    result = this->glyphBuffer.setUtf8Text(utf8.c_str(), utf8.size());

    if (result != BL_SUCCESS || this->glyphBuffer.size() != StringLength(utf8)) {
        this->glyphBuffer.clear();
        return;
    }

    ApplyTransform(this->glyphBuffer.content(), this->glyphBuffer.size(), style->textTransform);

    if (this->font.blFont.shape(this->glyphBuffer) != BL_SUCCESS) {
        return;
    }

    this->LayoutText(utf8, style, maxWidth, maxHeight);
}

void TextBlock::LayoutText(const std::string& utf8, Style* style, float maxWidth, float maxHeight) {
    /*
     * Layout a string of characters given the maxWidth and maxHeight bounds.
     *
     * The string of characters are split into two buffers (1:1 mapping): a string of utf8 encoded codepoints and
     * a glyph buffer with glyph id (font specific character ids). The TextIterator hides most of the details of
     * syncing the two buffers.
     *
     * The glyph buffer is effectively immutable. Each line will be a view of a section or run of the buffer.
     *
     * The layout has to do the following:
     * - Do not exceed maxWidth
     * - Do not exceed maxHeight
     * - Trim leading spaces
     * - Trim trailing spaces
     * - If layout needs to move to the next line, perform the line break at the last space.
     * - If no more characters fit in the layout, apply text overflow policy: Ellipsis or a hard clip/break.
     * - Ensure each line "view" correctly refers to the immutable glyph buffer.
     */

    const TextIterator textIteratorEnd(this->glyphBuffer.size(), utf8.end());
    TextIterator walker = this->TrimLeft({ 0, utf8.begin() }, textIteratorEnd);
    TextIterator lineStart = walker;
    TextIterator lastSpace = textIteratorEnd;
    const std::size_t maxLines = style->maxLines.AsInt32(0);
    double lineWidth{};

    while (walker != textIteratorEnd) {
        const auto c{ walker.CodepointAsChar() };
        const auto glyphAdvance{ this->GetGlyphAdvance(walker.GlyphBufferIndex()) };

        if (c == ' ') {
            // Record the last seen space. Back to back spaces are ok because lastSpace is only used to
            // break a line. After a break, the line is trimmed left and right.
            lastSpace = walker;
            lineWidth += glyphAdvance;
            walker++;
        } else if (c == '\n') {
            // Newline character. Break now.
            this->PushLine(lineStart, walker);

            // More characters exist, but no vertical space is left. Ellipsize and bail.
            if (this->AtVerticalLimit(maxHeight, maxLines)) {
                EllipsizeIfNecessary(style, maxWidth);
                lineStart = textIteratorEnd;
                break;
            }

            // Proceed. Increment walker so the newline is not processed again.
            lineWidth = 0;
            lastSpace = textIteratorEnd;
            lineStart = walker++;
        } else if (lineWidth + glyphAdvance >= maxWidth) {
            // Character does not fit on this line. Break at lastSpace, if a space was encountered, otherwise, break
            // at this character.
            if (lastSpace != textIteratorEnd) {
                this->PushLine(lineStart, lastSpace);
                // This rewinds walker and some characters will be processed again, but I am OK with the extra
                // processing to keep this layout code manageable.
                walker = lastSpace;
                lastSpace = textIteratorEnd;
            } else {
                this->PushLine(lineStart, walker);
            }

            // More characters exist, but no vertical space is left. Ellipsize and bail.
            if (this->AtVerticalLimit(maxHeight, maxLines)) {
                EllipsizeIfNecessary(style, maxWidth);
                lineStart = textIteratorEnd;
                break;
            }

            // Process to a new line. Ensure the new line processing starts without any leading spaces.
            lineWidth = 0;
            lastSpace = textIteratorEnd;
            walker = this->TrimLeft(walker, textIteratorEnd);
            lineStart = walker;
        } else {
            // Move the lineWidth and walker.
            lineWidth += glyphAdvance;
            walker++;
        }
    }

    // Last line did not exceed maxWidth, so add it to lines.
    if (lineStart != textIteratorEnd) {
        this->PushLine(lineStart, textIteratorEnd);
    }

    float width{ 0 };

    for (auto& line : this->lines) {
        width = std::max(width, line.width);
        line.textAlign = style->textAlign;
    }

    // Round up so sub-pixels can be rendered to the int texture dimensions.
    this->calculatedWidth = ::ceilf(width);
    this->calculatedHeight = ::ceilf(static_cast<float>(this->lines.size()) * this->font.lineHeight());
}

void TextBlock::Paint(RenderingContext2D* context) {
    if (this->IsEmpty()) {
        return;
    }

    auto target{ this->EnsureLockableTexture(context->renderer, this->calculatedWidth, this->calculatedHeight) };

    if (!target) {
        LOG_ERROR("Failed to create paint texture.");
        return;
    }

    auto pixels{ target.Lock() };
    Timer t("text render");

    context->Begin(pixels.Data(), pixels.Width(), pixels.Height(), pixels.Pitch());

    // Fill entire texture surface with transparent to start from a known state.
    context->SetColor(0);
    context->FillAll();

    auto y{ this->font.ascent() };

    context->SetColor(ColorWhite);
    context->SetFont(this->font.blFont);

    for (std::size_t i = 0; i < this->lines.size(); i++) {
        const auto& line = lines[i];
        float x;

        // The lines need to be aligned on the surface, but the surface may be smaller than the box bounds. So, the
        // box will need to also apply alignment on the surface during composite.
        switch (line.textAlign) {
            case StyleTextAlignCenter:
                x = (target.Width() - line.width) / 2.f;
                break;
            case StyleTextAlignRight:
                x = target.Width() - line.width;
                break;
            case StyleTextAlignLeft:
            default:
                x = 0;
                break;
        }

        if (line.glyphRun.size) {
            context->FillText(x, y, line.glyphRun);
        }

        if (line.ellipsis) {
            // BLGlyphBuffer is immutable (to appends), so just draw the ellipsis after the rendered line.
            context->FillText(x + line.width - this->font.ellipsisWidth(), y, "...");
        }

        y += this->font.lineHeight();
    }

    context->End();

    pixels.Release();
}

int32_t TextBlock::Width() const noexcept {
    return this->calculatedWidth;
}

int32_t TextBlock::Height() const noexcept {
    return this->calculatedHeight;
}

float TextBlock::WidthF() const noexcept {
    return this->calculatedWidth;
}

float TextBlock::HeightF() const noexcept {
    return this->calculatedHeight;
}

void TextBlock::Invalidate() noexcept {
    this->calculatedWidth = this->calculatedHeight = 0;
    lines.clear();
}

bool TextBlock::IsEmpty() const noexcept {
    return this->calculatedWidth <= 0 || this->calculatedHeight <= 0;
}

float TextBlock::GetRunWidth(const BLGlyphRun& run) const noexcept {
    const auto scaleFactor{ this->font.scaleX() };
    const auto placementData{ run.placementDataAs<BLGlyphPlacement>() };
    double len{0};

    for (std::size_t i = 0; i < run.size; i++) {
        len += (placementData[i].advance.x * scaleFactor);
    }

    return static_cast<float>(len);
}

double TextBlock::GetGlyphAdvance(std::size_t glyphBufferIndex) const noexcept {
    return this->glyphBuffer.placementData()[glyphBufferIndex].advance.x * this->font.scaleX();
}

BLGlyphRun TextBlock::GetGlyphRun(std::size_t start, std::size_t end) const noexcept {
    auto run{ this->glyphBuffer.glyphRun() };

    run.glyphData = &run.glyphDataAs<uint32_t>()[start];
    run.placementData = &run.placementDataAs<BLGlyphPlacement>()[start];
    run.size = end - start;

    return run;
}

static std::size_t StringLength(const std::string& utf8) noexcept {
    auto i{ utf8.begin() };
    std::size_t length{};

    while (i != utf8.end()) {
        utf8::unchecked::next(i);
        length++;
    }

    return length;
}

static void ApplyTransform(uint32_t* codepoints, std::size_t size, StyleTextTransform transform) noexcept {
    int (*op)(int);

    switch (transform) {
        case StyleTextTransformLowercase:
            op = std::tolower;
            break;
        case StyleTextTransformUppercase:
            op = std::toupper;
            break;
        default:
            return;
    }

    for (std::size_t i = 0; i < size; i++) {
        if (codepoints[i] < 255) {
            codepoints[i] = op(static_cast<int32_t>(codepoints[i]));
        }
    }
}

TextBlock::TextIterator TextBlock::TrimLeft(const TextIterator& begin, const TextIterator& end) const noexcept {
    auto i = begin;

    while (i != end && i.CodepointAsChar() == ' ') {
        i++;
    }

    return i;
}

void TextBlock::PushLine(const TextIterator& begin, const TextIterator& end) {
    auto i = begin;
    auto lastNonSpace = end;

    while (i != end) {
        if (i.CodepointAsChar() != ' ') {
            lastNonSpace = i;
        }
        i++;
    }

    if (lastNonSpace != end) {
        lastNonSpace++;
    }

    auto run{ this->GetGlyphRun(begin.GlyphBufferIndex(), lastNonSpace.GlyphBufferIndex()) };

    this->lines.push_back({ run, this->GetRunWidth(run), false, StyleTextAlignLeft });
}

bool TextBlock::AtVerticalLimit(float maxHeight, std::size_t maxLines) const noexcept {
    return this->AtVerticalLimit(maxHeight, maxLines, this->lines.size());
}

bool TextBlock::AtVerticalLimit(float maxHeight, std::size_t maxLines, std::size_t lineNo) const noexcept {
    return (maxLines > 0 && lineNo == maxLines) || (maxHeight > 0 && (lineNo * this->font.lineHeight()) > maxHeight);
}

void TextBlock::EllipsizeIfNecessary(Style* style, float maxWidth) noexcept {
    /*
     * Assumes that layout processing has completed and the text could not fit in the
     * layout area. If text overflow is not ellipsis, the last line is sufficiently
     * cropped. If text overflow is ellipsis, a "..." should be placed at the end
     * of the line. If there is not enough room for "...", characters need to be popped
     * off the back of the glyph buffer until space is available.
     *
     * Since the glyph buffer is not dynamic, the "..." cannot be appended. So, the
     * size is adjusted and line width are adjusted to fit the ellipsis. The Paint()
     * method will just draw the text immediately following the line.
     *
     * The algorithm has two minot rendering issues (my opinion). 1) A line can
     * contain only "...". This is fine for single line text, but it looks weird  in
     * some multiline scenarios. 2) Spaces can precede the ellipsis (ex: My Text ...).
     * If a run of multiple spaces are in the line, the ellipsis appears dangling.
     *
     * The font caches the ellipsis width. An ellipsis is considered to be three
     * consecutive '.' characters, not the unicode ellipsis character.
     */

    if (style->textOverflow != StyleTextOverflowEllipsis) {
        return;
    }

    // Bail if no ellipsis width or no room in maxWidth.
    const auto ellipsisWidth{ this->font.ellipsisWidth() };

    if (ellipsisWidth <= 0 || maxWidth < ellipsisWidth) {
        return;
    }

    auto& lastLine{ this->lines.back() };
    auto newWidth{ lastLine.width };

    lastLine.ellipsis = true;

    // Pop characters until there is enough space.
    for (int32_t i = static_cast<int32_t>(lastLine.glyphRun.size) - 1; i >= 0; i--) {
        if (newWidth + ellipsisWidth <= maxWidth) {
            lastLine.width = (newWidth + ellipsisWidth);
            lastLine.glyphRun.size = i + 1;
            return;
        }

        newWidth -= (lastLine.glyphRun.placementDataAs<BLGlyphPlacement>()[i].advance.x * this->font.scaleX());
    }

    // Not enough space, so just display the ellipsis for this line.
    lastLine.width = ellipsisWidth;
    lastLine.glyphRun.size = 0;
}

TextBlock::TextIterator::TextIterator(std::size_t glyphBufferIndex, const string_iterator& utf8) noexcept
: glyphBufferIndex(glyphBufferIndex), utf8(utf8) {
}

uint32_t TextBlock::TextIterator::Codepoint() const noexcept {
    auto copy = this->utf8;
    return utf8::unchecked::next(copy);
}

char TextBlock::TextIterator::CodepointAsChar() const noexcept {
    const auto codepoint{ this->Codepoint() };

    return codepoint < 255 ? static_cast<char>(codepoint) : '\0';
}

std::size_t TextBlock::TextIterator::GlyphBufferIndex() const noexcept {
    return this->glyphBufferIndex;
}

bool TextBlock::TextIterator::operator!=(const TextIterator& other) const noexcept {
    return this->glyphBufferIndex != other.glyphBufferIndex;
}

bool TextBlock::TextIterator::operator==(const TextIterator& other) const noexcept {
    return this->glyphBufferIndex == other.glyphBufferIndex;
}

TextBlock::TextIterator TextBlock::TextIterator::operator++(int32_t) noexcept {
    this->glyphBufferIndex++;
    utf8::unchecked::next(this->utf8);

    return *this;
}

} // namespace ls
