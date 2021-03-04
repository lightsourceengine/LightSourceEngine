/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/TextBlock.h>

#include <lse/Renderer.h>
#include <lse/FontManager.h>
#include <lse/Style.h>
#include <lse/StyleContext.h>
#include <lse/Timer.h>
#include <lse/PixelConversion.h>
#include <lse/Log.h>
#include <lse/string-ext.h>
#include <lse/math-ext.h>
#include <cmath>
#include <utf8.h>
#include <cctype>
#include <utility>

namespace lse {

void TextBlock::Shape(
    const std::string& utf8, Font* font, Style* style, StyleContext* context,
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
  this->Invalidate();

  if (utf8.empty() || !style || !font || font->GetFontStatus() != FontStatusReady) {
    return;
  }

  this->font = font->GetFontSourceAs<FTFontSource>();
  this->fontSize = SnapToPixelGrid<int32_t>(context->ComputeFontSize(style));

  if (this->fontSize <= 0 || !this->font->SetFontSizePt(this->fontSize)) {
    return;
  }

  this->LoadCodepoints(utf8, style);

  this->Layout(style, maxWidth, maxHeight);
}

void TextBlock::Layout(Style* style, float maxWidth, float maxHeight) {
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

  const auto textIteratorEnd = this->codepoints.size();
  auto walker = this->TrimLeft(0, textIteratorEnd);
  auto lineStart = walker;
  auto lastSpace = textIteratorEnd;
  const std::size_t maxLines = style->GetInteger(StyleProperty::maxLines).value_or(0);
  Float266 lineWidth266{};
  Float266 advance266;
  Float266 maxWidth266{ToFloat266(maxWidth)};
  Float266 maxHeight266{ToFloat266(maxHeight)};
  uint32_t c;

  while (walker != textIteratorEnd) {
    c = this->codepoints[walker].value;
    advance266 = this->font->GetAdvance(c);

    if (c == ' ') {
      // Record the last seen space. Back to back spaces are ok because lastSpace is only used to
      // break a line. After a break, the line is trimmed left and right.
      lastSpace = walker;
      lineWidth266 += advance266;
      walker++;
    } else if (c == '\n') {
      // Newline character. Break now.
      this->PushLine(lineStart, walker);

      // More characters exist, but no vertical space is left. Ellipsize and bail.
      if (this->AtVerticalLimit(maxHeight266, maxLines)) {
        this->EllipsizeIfNecessary(style, maxWidth);
        lineStart = textIteratorEnd;
        break;
      }

      // Proceed. Increment walker so the newline is not processed again.
      lineWidth266 = 0;
      lastSpace = textIteratorEnd;
      lineStart = ++walker;
    } else if (lineWidth266 + advance266 >= maxWidth266) {
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
      if (this->AtVerticalLimit(maxHeight266, maxLines)) {
        this->EllipsizeIfNecessary(style, maxWidth266);
        lineStart = textIteratorEnd;
        break;
      }

      // Process to a new line. Ensure the new line processing starts without any leading spaces.
      lineWidth266 = 0;
      lastSpace = textIteratorEnd;
      walker = this->TrimLeft(walker, textIteratorEnd);
      lineStart = walker;
    } else {
      // Move the lineWidth and walker.
      lineWidth266 += advance266;
      walker++;
    }
  }

  // Last line did not exceed maxWidth, so add it to lines.
  if (lineStart != textIteratorEnd) {
    this->PushLine(lineStart, textIteratorEnd);
  }

  maxWidth266 = 0;

  for (auto& line : this->lines) {
    maxWidth266 = std::max(maxWidth266, line.width266 = this->MeasureLine(line));
  }

  const auto lineHeight = ::ceilf(FromFloat266(this->font->GetLineHeight()));

  // Round up so sub-pixels can be rendered to the int texture dimensions.
  this->calculatedWidth = ::ceilf(FromFloat266(maxWidth266));
  this->calculatedHeight = lineHeight * static_cast<float>(this->lines.size());
  this->align = style->GetEnum<StyleTextAlign>(StyleProperty::textAlign);
}

void TextBlock::Paint(Renderer* renderer) {
  if (this->IsEmpty() || !this->font->SetFontSizePt(this->fontSize)) {
    return;
  }

  auto textureLock{this->LockTexture(renderer)};

  if (!textureLock.IsLocked()) {
    return;
  }

  int32_t x;
  int32_t y{};
  const auto surface{reinterpret_cast<color_t*>(textureLock.GetPixels())};
  const auto lineHeight{this->ComputeLineHeight()};

  for (const auto& line : this->lines) {
    switch (this->align) {
      case StyleTextAlignCenter:
        x = SnapToPixelGrid<int32_t>((this->WidthF() - this->MeasureLineF(line)) / 2.f);
        break;
      case StyleTextAlignRight:
        x = SnapToPixelGrid<int32_t>(this->WidthF() - this->MeasureLineF(line));
        break;
      default:
        x = 0;
        break;
    }

    this->PaintLine(line, x, surface + (y * textureLock.GetWidth()), textureLock.GetWidth());

    y += lineHeight;
  }

  ConvertToFormat(surface, this->texture->Width() * this->texture->Height(), this->texture->Format());

  this->isReady = true;
}

void TextBlock::PaintLine(const TextLine& line, int32_t x, color_t* surface, int32_t pitch) noexcept {
  FT_BitmapGlyph glyph;
  color_t* s;
  int32_t yOffset;
  float advance;
  auto pos{static_cast<float>(x)};
  const auto ascent{::ceilf(FromFloat266(this->font->GetAscent()))};

  for (size_t i = line.start; i < line.end; i++) {
    advance = FromFloat266(this->codepoints[i].advance266);
    glyph = this->font->GetGlyphBitmap(this->codepoints[i].value);

    if (!glyph) {
      pos += advance;
      continue;
    }

    yOffset = static_cast<int32_t>(ascent - glyph->top);

    if (yOffset < 0) {
      continue;
    }

    s = surface + ((yOffset * pitch) + SnapToPixelGrid<int32_t>(pos) + glyph->left);

    for (uint32_t by = 0; by < glyph->bitmap.rows; by++) {
      for (uint32_t bx = 0; bx < glyph->bitmap.width; bx++) {
        *s++ = 0xFFFFFF | (glyph->bitmap.buffer[by*glyph->bitmap.pitch + bx] << 24);
      }
      s += (pitch - glyph->bitmap.width);
    }

    pos += advance;
  }
}

int32_t TextBlock::ComputeLineHeight() const noexcept {
  Float266 lineHeight{this->font->GetLineHeight()};

  if ((lineHeight & 0b111111) > 0) {
    return (lineHeight >> 6) + 1;
  }

  return (lineHeight >> 6);
}

TextureLock TextBlock::LockTexture(Renderer* renderer) noexcept {
  if (this->texture && this->Width() <= this->texture->Width() && this->Height() >= this->texture->Height()) {
    return {this->texture, true};
  }

  Texture::SafeDestroy(this->texture);

  const auto extraW{static_cast<int32_t>(this->Width() * 0.1f)};
  const auto extraH{static_cast<int32_t>(this->Width() * 0.05f)};

  this->texture = renderer->CreateTexture(this->Width() + extraW, this->Height() + extraH, Texture::Lockable);

  if (this->texture) {
    return {this->texture, true};
  }

  return {nullptr, false};
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

void TextBlock::Destroy() noexcept {
  this->texture = Texture::SafeDestroy(this->texture);
}

void TextBlock::Invalidate() noexcept {
  this->calculatedWidth = this->calculatedHeight = 0;
  this->lines.clear();
  this->font = nullptr;
  this->codepoints.clear();
  this->align = {};
  this->isReady = false;
}

bool TextBlock::IsEmpty() const noexcept {
  return this->calculatedWidth <= 0 || this->calculatedHeight <= 0;
}

void TextBlock::LoadCodepoints(const std::string& utf8, Style* style) {
  assert(!utf8.empty());

  auto i{utf8.begin()};
  uint32_t codepoint;
  int (*op)(int);

  this->codepoints.reserve(LengthUtf8(utf8));

  switch (style->GetEnum<StyleTextTransform>(StyleProperty::textTransform)) {
    case StyleTextTransformLowercase:
      op = &std::tolower;
      break;
    case StyleTextTransformUppercase:
      op = &std::toupper;
      break;
    default:
      op = nullptr;
      break;
  }

  while (i != utf8.end()) {
    codepoint = utf8::unchecked::next(i);

    if (op && codepoint < 255) {
      codepoint = op(static_cast<int32_t>(codepoint));
    }

    this->codepoints.emplace_back(codepoint, this->font->GetAdvance(codepoint));
  }

  if (this->font->HasKerning() && !this->codepoints.empty()) {
    auto size{this->codepoints.size() - 1};

    for (std::size_t c = 0; c < size; c++) {
      this->codepoints[c].advance266 += this->font->GetKerningX(
          this->codepoints[c].value, this->codepoints[c + 1].value);
    }
  }
}

size_t TextBlock::TrimLeft(std::size_t begin, std::size_t end) const noexcept {
  for (auto i = begin; i < end; i++) {
    if (this->codepoints[i].value != ' ') {
      return i;
    }
  }

  return end;
}

void TextBlock::PushLine(std::size_t begin, std::size_t end) {
  auto i = begin;
  auto lastNonSpace = end;

  while (i < end) {
    if (this->codepoints[i].value != ' ') {
      lastNonSpace = i;
    }
    i++;
  }

  if (lastNonSpace != end) {
    lastNonSpace++;
  }

  this->lines.emplace_back(begin, lastNonSpace);
}

int32_t TextBlock::MeasureLine(const TextLine& line) const noexcept {
  Float266 width266{};

  for (auto i = line.start; i < line.end; i++) {
    width266 += this->codepoints[i].advance266;
  }

  return width266;
}

float TextBlock::MeasureLineF(const TextLine& line) const noexcept {
  return FromFloat266(this->MeasureLine(line));
}

bool TextBlock::AtVerticalLimit(int32_t maxHeight266, std::size_t maxLines) const noexcept {
  return this->AtVerticalLimit(maxHeight266, maxLines, this->lines.size());
}

bool TextBlock::AtVerticalLimit(int32_t maxHeight266, std::size_t maxLines, std::size_t lineNo) const noexcept {
  return (maxLines > 0 && lineNo == maxLines) || (maxHeight266 > 0 && ToFloat266(lineNo) > maxHeight266);
}

void TextBlock::EllipsizeIfNecessary(Style* style, int32_t maxWidth266) noexcept {
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

  if (style->GetEnum(StyleProperty::textOverflow) != StyleTextOverflowEllipsis) {
    return;
  }

  // Bail if no ellipsis width or no room in maxWidth.
  const auto ellipsisWidth266{ this->font->GetAdvance('.') * (3 << 6) };

  if (ellipsisWidth266 <= 0 || maxWidth266 < ellipsisWidth266) {
    return;
  }

  auto& lastLine{ this->lines.back() };

  if (lastLine.end == 0) {
    return;
  }

  Float266 space266{};

  lastLine.ellipsis = true;

  // Pop characters until there is enough space.
  for (auto i = lastLine.end - 1; i >= 0; i--) {
    space266 += this->codepoints[i].advance266;

    if (space266 >= ellipsisWidth266) {
      lastLine.end = i;
      return;
    }
  }

  lastLine.end = lastLine.start;
}

bool TextBlock::IsReady() const noexcept {
  return this->isReady;
}

Texture* TextBlock::GetTexture() const noexcept {
  return this->texture;
}

IntRect TextBlock::GetTextureSourceRect() const noexcept {
  return {0, 0, this->calculatedWidth, this->calculatedHeight};
}

} // namespace lse
