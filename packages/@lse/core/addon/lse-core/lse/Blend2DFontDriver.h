/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/FontDriver.h>

namespace lse {

/**
 * FontManager plugin that using Blend2D to load fonts.
 */
class Blend2DFontDriver : public FontDriver {
 public:
  FontSource* LoadFontSource(void* data, size_t dataSize, int32_t index) override;
  void DestroyFontSource(FontSource* fontSource) override;
};

} // namespace lse
