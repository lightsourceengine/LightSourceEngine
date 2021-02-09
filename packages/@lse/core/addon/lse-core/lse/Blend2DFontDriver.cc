/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Blend2DFontDriver.h"

#include <blend2d.h>
#include <lse/Blend2DFontFace.h>
#include <lse/Log.h>

namespace lse {

FontSource* Blend2DFontDriver::LoadFontSource(void* data, size_t dataSize, int32_t index) {
  uint8_t* dataCopy{new (std::nothrow) uint8_t[dataSize]};

  if (!dataCopy) {
    LOG_ERROR("Failed to allocate memory for TTF file.");
    return {};
  }

  memcpy(dataCopy, data, dataSize);

  BLFontData fontData{};
  BLDestroyImplFunc destroy = [](void* impl, void* destroyData) {
    delete [] static_cast<uint8_t*>(destroyData);
  };
  BLResult result{fontData.createFromData(dataCopy, dataSize, destroy, dataCopy)};

  if (result != BL_SUCCESS) {
    LOG_ERROR("BLFontData.createFromData: %#010x", result);
    delete [] dataCopy;
    return {};
  }

  BLFontFace fontFace{};
  result = fontFace.createFromData(fontData, index);

  if (result != BL_SUCCESS) {
    LOG_ERROR("createFromFile: %#010x", result);
    return {};
  }

  return new Blend2DFontFace(std::move(fontFace));
}

FontSource* Blend2DFontDriver::LoadFontSource(const char* file, int32_t index) {
  BLFontFace fontFace{};
  auto result{fontFace.createFromFile(file, index)};

  if (result != BL_SUCCESS) {
    LOG_ERROR("createFromFile: %#010x", result);
    return {};
  }

  return new Blend2DFontFace(std::move(fontFace));
}

void Blend2DFontDriver::DestroyFontSource(FontSource* fontSource) {
  if (fontSource) {
    delete static_cast<Blend2DFontFace*>(fontSource);
  }
}

} // namespace lse
