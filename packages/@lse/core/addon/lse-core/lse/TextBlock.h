/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
  // Render the text block. Shape must be called for this function to render.
  void Paint(Renderer* renderer);
  // Layout text according to style settings and dimensions of the bounding box.
  void Shape(const std::string& utf8, Font* font, Style* style, StyleContext* context, float maxWidth, float maxHeight);
  // Reset text layout, usually due to a text change or style change.
  void Invalidate() noexcept;
  // Invalidate and destroy the texture that backs this block. Object can be reused after Destroy().
  void Destroy() noexcept;
  // Is the layout empty? It will be empty if Shape() has not been called or Invalidate() was called.
  bool IsEmpty() const noexcept;
  // Is the text block's backing texture ready to be rendered?
  bool IsReady() const noexcept;
  // Get the texture backing this text block. Must ensure IsReady() is true before accessing the texture.
  Texture* GetTexture() const noexcept;
  // Get the source rect of the texture.
  IntRect GetTextureSourceRect() const noexcept;
  // Calculated bounds of the text. Set after call to Shape().
  int32_t Width() const noexcept;
  int32_t Height() const noexcept;
  float WidthF() const noexcept;
  float HeightF() const noexcept;

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

    // codepoint value (not the glyph id!)
    uint32_t value{};
    // scaled advance value, including kerning
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
  void PaintLine(const TextLine& line, float x, color_t* surface, int32_t pitch) noexcept;
  int32_t ComputeLineHeight() const noexcept;
  TextureLock LockTexture(Renderer* renderer) noexcept;
  void AppendEllipsis(TextLine& line) noexcept;

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
