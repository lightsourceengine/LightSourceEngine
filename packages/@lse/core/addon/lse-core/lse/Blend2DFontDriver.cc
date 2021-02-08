/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Blend2DFontDriver.h"

#include <blend2d.h>
#include <lse/Log.h>

namespace lse {

static FontSource* LoadBLFontFaceCore(BLFontDataCore* fontData, int32_t index) noexcept;

FontSource* Blend2DFontDriver::LoadFontSource(void* data, size_t dataSize, int32_t index) {
  BLResult result;
  BLFontDataCore fontData{};

  result = blFontDataInit(&fontData);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontDataInit: %#010x", result);
    return {};
  }

  auto dataCopy{new uint8_t[dataSize]};

  memccpy(dataCopy, static_cast<uint8_t*>(data), 1, dataSize);

  result = blFontDataCreateFromData(
      &fontData,
      data,
      dataSize,
      [](void* impl, void* destroyData) {
        delete [] static_cast<uint8_t*>(destroyData);
      },
      dataCopy);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontDataCreateFromData: %#010x", result);
    blFontDataDestroy(&fontData);
    delete [] dataCopy;
    return {};
  }

  return LoadBLFontFaceCore(&fontData, index);
}

FontSource* Blend2DFontDriver::LoadFontSource(const char* file, int32_t index) {
  BLResult result;
  BLFontDataCore fontData{};

  result = blFontDataInit(&fontData);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontDataInit: %#010x", result);
    return {};
  }

  result = blFontDataCreateFromFile(&fontData, file, 0);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontDataCreateFromData: %#010x", result);
    blFontDataDestroy(&fontData);
    return {};
  }

  return LoadBLFontFaceCore(&fontData, index);
}

void Blend2DFontDriver::DestroyFontSource(FontSource* fontSource) {
  if (fontSource) {
    BLFontFaceCore fontFace{ static_cast<BLFontFaceImpl*>(fontSource) };
    blFontFaceDestroy(&fontFace);
  }
}

static FontSource* LoadBLFontFaceCore(BLFontDataCore* fontData, int32_t index) noexcept {
  BLFontFaceCore fontFace{};

  BLResult result = blFontFaceInit(&fontFace);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontFaceInit: %#010x", result);
    blFontDataDestroy(fontData);
    return {};
  }

  result = blFontFaceCreateFromData(&fontFace, fontData, index);

  if (result != BL_SUCCESS) {
    LOG_ERROR("blFontFaceCreateFromData: %#010x", result);
    blFontDataDestroy(fontData);
    return {};
  }

  return fontFace.impl;
}

} // namespace lse
