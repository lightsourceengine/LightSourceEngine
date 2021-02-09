/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Blend2DFontFace.h"

#include <lse/Math.h>
#include <lse/Log.h>

namespace lse {

constexpr auto kMaxFonts = 10;

Blend2DFontFace::Blend2DFontFace(BLFontFace&& fontFace) : fontFace(std::move(fontFace)) {
}

Blend2DFont Blend2DFontFace::GetFontBySize(float fontSize) {
  if (std::isnan(fontSize) || fontSize <= 0.f) {
    return {};
  }

  for (auto p = this->fontsBySize.begin(); p != this->fontsBySize.end(); p++) {
    if (lse::Equals(fontSize, p->blFont.size())) {
      if (p != this->fontsBySize.begin()) {
        // remove element p and put it at the front of the list (without a copy)
        this->fontsBySize.splice(this->fontsBySize.begin(), this->fontsBySize, p, std::next(p));
      }

      return *p;
    }
  }

  Blend2DFont font{};

  if (font.blFont.createFromFace(this->fontFace, fontSize) == BL_SUCCESS) {
    BLTextMetrics ellipsisTextMetrics{};
    BLGlyphBuffer ellipsis{};

    ellipsis.setUtf8Text("...");
    font.blFont.shape(ellipsis);
    font.blFont.getTextMetrics(ellipsis, ellipsisTextMetrics);
    font._ellipsisWidth = static_cast<float>(ellipsisTextMetrics.advance.x);

    this->fontsBySize.push_front(font);

    if (this->fontsBySize.size() > kMaxFonts) {
      this->fontsBySize.pop_back();
    }
  } else {
    LOG_WARN("Failed to load font for size %f", fontSize);
  }

  return font;
}

} // namespace lse
