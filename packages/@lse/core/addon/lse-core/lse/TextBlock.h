/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <vector>
#include <blend2d.h>
#include <lse/Paintable.h>
#include <lse/Resources.h>
#include <lse/StyleEnums.h>
#include <lse/Blend2DFontFace.h>

namespace lse {

class Renderer;
class RenderingContext2D;
class Style;
class StyleContext;
class Font;

/**
 * Drawing and layout of text strings for the content area of text elements.
 */
class TextBlock final : public Paintable {
 public:
  ~TextBlock() override = default;

  void Paint(RenderingContext2D* context) override;

  /**
   * Layout the text according to the style policy and dimensions.
   */
  void Shape(
      const std::string& utf8, Font* font, Style* style, StyleContext* context,
      float maxWidth, float maxHeight);

  // Calculated bounds of the text. Set after call to Shape().

  int32_t Width() const noexcept;
  int32_t Height() const noexcept;
  float WidthF() const noexcept;
  float HeightF() const noexcept;

  /**
   * Reset the bounds.
   */
  void Invalidate() noexcept;

  /**
   * If empty, no bounds are set and the text block cannot be pained.
   */
  bool IsEmpty() const noexcept;

 private:
  using string_iterator = decltype(std::declval<const std::string>().begin());

  // Iterator that combines BLGlyphBuffer iterator and utf8 string iterator, keeping them in sync.
  class TextIterator {
   public:
    TextIterator(std::size_t glyphBufferIndex, const string_iterator& utf8) noexcept;

    uint32_t Codepoint() const noexcept;
    char CodepointAsChar() const noexcept;
    std::size_t GlyphBufferIndex() const noexcept;

    bool operator!=(const TextIterator& other) const noexcept;
    bool operator==(const TextIterator& other) const noexcept;
    TextBlock::TextIterator operator++(int32_t) noexcept;

   private:
    std::size_t glyphBufferIndex{};
    string_iterator utf8{};
  };

  // Layout of a line of text. Used by paint to draw the text of the line.
  struct TextLine {
    // Text line as font glyph ids (not utf8 characters!)
    BLGlyphRun glyphRun;
    // Width of the line (screen pixels), scaled to the size of the font.
    float width;
    // If true, ellipsis "..." will be draw immediately after this line.
    bool ellipsis;
    // The alignment of the line on the Paintable taret texture.
    StyleTextAlign textAlign;
  };

 private:
  float GetRunWidth(const BLGlyphRun& run) const noexcept;
  double GetGlyphAdvance(std::size_t glyphBufferIndex) const noexcept;
  BLGlyphRun GetGlyphRun(std::size_t start, std::size_t end) const noexcept;
  void LayoutText(const std::string& utf8, Style* style, float maxWidth, float maxHeight);
  TextIterator TrimLeft(const TextIterator& begin, const TextIterator& end) const noexcept;
  void PushLine(const TextIterator& begin, const TextIterator& end);
  bool AtVerticalLimit(float maxHeight, std::size_t maxLines) const noexcept;
  bool AtVerticalLimit(float maxHeight, std::size_t maxLines, std::size_t lineNo) const noexcept;
  void EllipsizeIfNecessary(Style* style, float maxWidth) noexcept;

 private:
  Blend2DFont font;
  BLGlyphBuffer glyphBuffer{};
  int32_t calculatedWidth{ 0 };
  int32_t calculatedHeight{ 0 };
  std::vector<TextLine> lines{};
};

} // namespace lse
