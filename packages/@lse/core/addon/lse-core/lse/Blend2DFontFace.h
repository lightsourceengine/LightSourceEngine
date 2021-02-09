/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <blend2d.h>
#include <list>

namespace lse {

class Blend2DFont {
 public:
  BLFont blFont{};
  float _ellipsisWidth{};

  auto ellipsisWidth() const noexcept { return this->_ellipsisWidth; }
  auto scaleX() const noexcept { return this->blFont.matrix().m00; }
  auto ascent() const noexcept { return this->blFont.metrics().ascent; }
  auto lineHeight() const noexcept {
    return this->blFont.metrics().ascent + this->blFont.metrics().descent + this->blFont.metrics().lineGap;
  }
  bool empty() const noexcept { return this->blFont.empty(); }
};

class Blend2DFontFace {
 public:
  Blend2DFontFace(BLFontFace&& fontFace);

  Blend2DFont GetFontBySize(float fontSize);

 private:
  BLFontFace fontFace{};
  // lru cache for font instances by size
  std::list<Blend2DFont> fontsBySize{};
};

} // namespace lse
