/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <memory>
#include <phmap.h>

namespace lse {

using FontSource = void;

/**
 * Plugin that delegates font loading from the FontManager.
 */
class FontDriver {
 public:
  static constexpr auto APP_DATA_KEY = "FontDriver";

  virtual ~FontDriver() = default;

  virtual FontSource* LoadFontSource(void* data, size_t dataSize, int32_t index) = 0;
  virtual FontSource* LoadFontSource(const char* file, int32_t index) = 0;
  virtual void DestroyFontSource(FontSource* fontSource) = 0;
};

} // namespace lse
