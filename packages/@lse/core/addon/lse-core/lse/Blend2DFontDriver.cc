/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Blend2DFontDriver.h"

#include <blend2d.h>
#include <lse/Log.h>

namespace lse {

FontSource* Blend2DFontDriver::LoadFontSource(void* data, size_t dataSize, int32_t index) {
  BLResult result;
  BLFontDataCore fontData{};

  result = blFontDataInit(&fontData);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontDataInit: %#010x", result);
    return {};
  }

  result = blFontDataCreateFromData(&fontData, data, dataSize, nullptr, nullptr);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontDataCreateFromData: %#010x", result);
    blFontDataDestroy(&fontData);
    return {};
  }

  BLFontFaceCore fontFace{};

  result = blFontFaceInit(&fontFace);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontFaceInit: %#010x", result);
    blFontDataDestroy(&fontData);
    return {};
  }


  result = blFontFaceCreateFromData(&fontFace, &fontData, index);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontFaceCreateFromData: %#010x", result);
    blFontDataDestroy(&fontData);
    return {};
  }

  return fontFace.impl;
}

void Blend2DFontDriver::DestroyFontSource(FontSource* fontSource) {
  if (fontSource) {
    BLFontFaceCore fontFace{ static_cast<BLFontFaceImpl*>(fontSource) };
    blFontFaceDestroy(&fontFace);
  }
}

} // namespace lse
