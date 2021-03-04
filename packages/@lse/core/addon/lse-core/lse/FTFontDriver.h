/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/FontDriver.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <phmap.h>
#include <lse/fs-ext.h>
#include <lse/math-ext.h>

namespace lse {

class FTFontSource;

/**
 * FontManager plugin that using FreeType to load fonts.
 */
class FTFontDriver : public FontDriver {
 public:
  FTFontDriver();
  ~FTFontDriver() override;

  FontSource* LoadFontSource(void* data, size_t dataSize, int32_t index) override;
  FontSource* LoadFontSource(const char* file, int32_t index) override;
  void DestroyFontSource(FontSource* fontSource) override;

 private:
  FTFontSource* NewFTFontSource(ByteArray&& font, int32_t index) noexcept;

 private:
  FT_Library library{};
  std::mutex createFaceLock{};
};

class FTFontSource {
 public:
  FTFontSource(FT_Face face, ByteArray&& memory) noexcept;
  ~FTFontSource() noexcept;

  FT_Face GetFace() const noexcept;
  uint32_t ToGlyphId(uint32_t codepoint) const noexcept;
  Float266 GetAdvance(uint32_t codepoint) const noexcept;
  bool SetFontSizePt(int32_t pointSize) noexcept;
  bool HasKerning() const noexcept;
  Float266 GetKerningX(uint32_t cp1, uint32_t cp2) const noexcept;
  Float266 GetAscent() const noexcept;
  Float266 GetLineHeight() const noexcept;
  FT_BitmapGlyph GetGlyphBitmap(uint32_t codepoint) const noexcept;

 private:
  struct Key {
    union {
      struct {
        uint32_t codepoint;
        int32_t fontSize;
      };
      uint64_t value;
    };

    Key() noexcept : value(0) {}
    Key(uint32_t codepoint, int32_t fontSize) noexcept : codepoint(codepoint), fontSize(fontSize) {}
  };

  FT_Glyph LoadGlyphBitmap(uint32_t codepoint) const noexcept;

 private:
  FT_Face face{};
  ByteArray memory{};
  int32_t currentPointSize{};
  Float266 ascent{};
  Float266 lineHeight{};

  // codepoint -> glyph id table
  mutable phmap::flat_hash_map<uint32_t, uint32_t> glyphIdCache;
  // codepoint + font size -> advance value
  mutable phmap::flat_hash_map<uint64_t, Float266> advanceCache;
  // codepoint + font size -> bitmap
  mutable phmap::flat_hash_map<uint64_t, FT_Glyph> bitmapCache;
};

} // namespace lse
