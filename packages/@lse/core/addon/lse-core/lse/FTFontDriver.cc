/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "FTFontDriver.h"

#include <memory>
#include <cctype>
#include <lse/Log.h>
#include <lse/fs-ext.h>
#include <freetype/ftadvanc.h>

namespace lse {

FTFontDriver::FTFontDriver() {
  auto error = FT_Init_FreeType(&this->library);

  if (error) {
    LOG_ERROR("%s", FT_Error_String(error));
    this->library = nullptr;
  }
}

FTFontDriver::~FTFontDriver() {
  if (this->library) {
    FT_Done_FreeType(this->library);
    this->library = nullptr;
  }
}

FontSource* FTFontDriver::LoadFontSource(void* data, size_t dataSize, int32_t index) {
  if (!this->library || !data || dataSize == 0) {
    return {};
  }

  return this->NewFTFontSource(NewByteArray(static_cast<uint8_t*>(data), dataSize), index);
}

FontSource* FTFontDriver::LoadFontSource(const char* file, int32_t index) {
  if (!this->library || !file) {
    return {};
  }

  return this->NewFTFontSource(ReadFileContents(file), index);
}

void FTFontDriver::DestroyFontSource(FontSource* fontSource) {
  if (fontSource) {
    auto ftFontSource{static_cast<FTFontSource*>(fontSource)};
    auto face{ftFontSource->GetFace()};

    {
      // create is done in another thread, docs say to have a mutex around face new/done
      std::lock_guard<std::mutex> guard(this->createFaceLock);
      FT_Done_Face(face);
    }

    delete ftFontSource;
  }
}

FTFontSource* FTFontDriver::NewFTFontSource(ByteArray&& font, int32_t index) noexcept {
  if (font.empty()) {
    return {};
  }

  FT_Face face{};
  FT_Error result;

  {
    // this may be called in a background thread, docs say to have a mutex around face new/done
    std::lock_guard<std::mutex> guard(this->createFaceLock);
    result = FT_New_Memory_Face(this->library, font.data(), font.size(), index, &face);
  }

  if (result == FT_Err_Ok) {
    if (FT_IS_SCALABLE(face)) {
      return new FTFontSource(face, std::move(font));
    } else {
      std::lock_guard<std::mutex> guard(this->createFaceLock);
      FT_Done_Face(face);
    }
  }

  return {};
}

FTFontSource::FTFontSource(FT_Face face, ByteArray&& memory) noexcept : face(face), memory(std::move(memory)) {
  this->glyphIdCache.reserve(1024);
  this->advanceCache.reserve(1024);
}

FTFontSource::~FTFontSource() noexcept {
  this->bitmapCache.ForEach([](const uint64_t& key, const FT_Glyph& glyph) {
    if (glyph) {
      FT_Done_Glyph(glyph);
    }
  });
}

FT_Face FTFontSource::GetFace() const noexcept {
  return this->face;
}

uint32_t FTFontSource::ToGlyphId(uint32_t codepoint) const noexcept {
  auto p{this->glyphIdCache.find(codepoint)};

  if (p == this->glyphIdCache.end()) {
    return (this->glyphIdCache[codepoint] = FT_Get_Char_Index(this->face, codepoint));
  }

  return p->second;
}

Float266 FTFontSource::GetAdvance(uint32_t codepoint) const noexcept {
  Key key{codepoint, this->currentPointSize};
  auto p{this->advanceCache.find(key.value)};
  FT_Fixed advance1616;

  if (p == this->advanceCache.end()) {
    // The fast path for get advance will be triggered under a handful of circumstances, FT_LOAD_NO_SCALE on
    // a scalable font is one of them. Otherwise, LoadGlyph() will be used, triggering a load of the outline, etc.
    if (FT_Get_Advance(this->face, this->ToGlyphId(codepoint), FT_LOAD_NO_SCALE, &advance1616)) {
      advance1616 = 0;
    }

    this->advanceCache[key.value] = advance1616;
  } else {
    advance1616 = p->second;
  }

  return FT_MulDiv(advance1616, this->face->size->metrics.x_scale, 64) >> 10;
}

bool FTFontSource::HasKerning() const noexcept {
  return FT_HAS_KERNING(this->face);
}

Float266 FTFontSource::GetKerningX(uint32_t cp1, uint32_t cp2) const noexcept {
  FT_Vector kerning;

  if (FT_Get_Kerning(this->face, this->ToGlyphId(cp1), this->ToGlyphId(cp2), FT_KERNING_DEFAULT, &kerning)) {
    return 0;
  } else {
    return kerning.x;
  }
}

Float266 FTFontSource::GetAscent() const noexcept {
  return this->ascent;
}

Float266 FTFontSource::GetLineHeight() const noexcept {
  return this->lineHeight;
}

FT_BitmapGlyph FTFontSource::GetGlyphBitmap(uint32_t codepoint) const noexcept {
  Key key{codepoint, this->currentPointSize};
  FT_Glyph glyph;

  auto ref{this->bitmapCache.Find(key.value)};

  if (ref.Empty()) {
    glyph = this->LoadGlyphBitmap(codepoint);
    this->bitmapCache.Insert(key.value, glyph);
  } else {
    glyph = ref.Get();
  }

  return reinterpret_cast<FT_BitmapGlyph>(glyph);
}

FT_Glyph FTFontSource::LoadGlyphBitmap(uint32_t codepoint) const noexcept {
  FT_Glyph glyph{};

  if (std::isspace(static_cast<int32_t>(codepoint))) {
    return {};
  }

  if (FT_Load_Glyph(this->face, this->ToGlyphId(codepoint), FT_LOAD_RENDER)) {
    return {};
  }

  if (FT_Get_Glyph(this->face->glyph, &glyph)) {
    return {};
  }

  if (glyph->format != FT_GLYPH_FORMAT_BITMAP) {
    FT_Done_Glyph(glyph);
    glyph = {};
  }

  return glyph;
}

bool FTFontSource::SetFontSizePt(int32_t pointSize) noexcept {
  if (this->currentPointSize == pointSize) {
    return true;
  }

  if (this->face && FT_Set_Char_Size(this->face, 0, pointSize*64, 0, 0) == 0) {
    this->ascent = static_cast<Float266>(this->face->size->metrics.ascender);
    this->lineHeight = static_cast<Float266>(this->face->size->metrics.height);
    this->currentPointSize = pointSize;

    return true;
  } else {
    this->ascent = 0;
    this->lineHeight = 0;
    this->currentPointSize = 0;

    return false;
  }
}

} // namespace lse
