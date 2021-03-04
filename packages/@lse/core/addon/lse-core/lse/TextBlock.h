/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <vector>
#include <lse/StyleEnums.h>
#include <lse/FTFontDriver.h>
#include <lse/Color.h>
#include <lse/Rect.h>
#include <lse/Texture.h>
#include <freetype/freetype.h>

namespace lse {

class Renderer;
class Style;
class StyleContext;
class Font;

/**
 * Drawing and layout of text strings for the content area of text elements.
 */
class TextBlock {
 public:
  void Paint(Renderer* renderer);

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

  void Destroy() noexcept;

  /**
   * If empty, no bounds are set and the text block cannot be pained.
   */
  bool IsEmpty() const noexcept;

  bool IsReady() const noexcept;

  Texture* GetTexture() const noexcept;
  IntRect GetTextureSourceRect() const noexcept;

 private:
  struct TextLine {
    TextLine() noexcept = default;
    TextLine(std::size_t start, std::size_t end) noexcept : start(start), end(end) {}

    std::size_t start{};
    std::size_t end{};
    Float266 width266{};
    bool ellipsis{};
  };

  struct Codepoint {
    Codepoint() noexcept = default;
    Codepoint(uint32_t value, Float266 advance) noexcept : value(value), advance266(advance) {}

    uint32_t value{};
    Float266 advance266{};
  };

 private:
  void LoadCodepoints(const std::string& utf8, Style* style);
  void Layout(Style* style, float maxWidth, float maxHeight);
  size_t TrimLeft(std::size_t begin, std::size_t end) const noexcept;
  void PushLine(std::size_t begin, std::size_t end);
  int32_t MeasureLine(const TextLine& line) const noexcept;
  float MeasureLineF(const TextLine& line) const noexcept;
  bool AtVerticalLimit(int32_t maxHeight266, std::size_t maxLines) const noexcept;
  bool AtVerticalLimit(int32_t maxHeight266, std::size_t maxLines, std::size_t lineNo) const noexcept;
  void EllipsizeIfNecessary(Style* style, int32_t maxWidth266) noexcept;
  void PaintLine(const TextLine& line, int32_t x, color_t* surface, int32_t pitch) noexcept;
  int32_t ComputeLineHeight() const noexcept;
  TextureLock LockTexture(Renderer* renderer) noexcept;

 private:
  FTFontSource* font{};
  int32_t fontSize{};
  Texture* texture{};
  int32_t calculatedWidth{};
  int32_t calculatedHeight{};
  StyleTextAlign align{};
  std::vector<Codepoint> codepoints{};
  std::vector<TextLine> lines{};
  bool isReady{false};
};

} // namespace lse
