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
#include <lse/LRUCache.h>

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

/**
 * Wrapper around a FreeType font face, with caching of glyph metrics and bitmaps.
 */
class FTFontSource {
 public:
  FTFontSource(FT_Face face, ByteArray&& memory) noexcept;
  ~FTFontSource() noexcept;

  // Get the font face
  FT_Face GetFace() const noexcept;
  // Does this font face have a kerning table? If not, GetKerningX() will always return 0.
  bool HasKerning() const noexcept;
  // Set the font point size to scale advance, kerning, ascent, line height and bitmap results
  bool SetFontSizePt(int32_t pointSize) noexcept;

  // Character advance, scaled to current font point size
  Float266 GetAdvance(uint32_t codepoint) const noexcept;
  // Kerning between characters, scaled to current font point size
  Float266 GetKerningX(uint32_t cp1, uint32_t cp2) const noexcept;
  // Font ascent, scaled to current font point size
  Float266 GetAscent() const noexcept;
  // Line height, scaled to current font point size
  Float266 GetLineHeight() const noexcept;
  // Get the bitmap, 8-bit grayscale, of the character
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
  uint32_t ToGlyphId(uint32_t codepoint) const noexcept;

 private:
  // font face object
  FT_Face face{};
  // font file backing the face. must be kept in memory until FT_Done_Face() is called.
  ByteArray memory{};
  // the currently selected font point size
  int32_t currentPointSize{};
  // font ascent, scaled with current point size
  Float266 ascent{};
  // font ascent, scaled with current point size
  Float266 lineHeight{};
  // codepoint -> glyph id table
  mutable phmap::flat_hash_map<uint32_t, uint32_t> glyphIdCache;
  // codepoint -> unscaled advance value
  mutable phmap::flat_hash_map<uint32_t, Float266> advanceCache;
  // codepoint + font size -> bitmap
  mutable LRUCache<uint64_t, FT_Glyph> bitmapCache{512};
};

} // namespace lse
