/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/FontDriver.h>
#include <freetype/freetype.h>

namespace lse {

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
  FT_Library library{};
  std::mutex lock{};
};

} // namespace lse
