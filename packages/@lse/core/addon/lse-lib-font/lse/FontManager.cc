/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "FontManager.h"

#include <lse/string-ext.h>
#include <std17/filesystem>

namespace lse {

// TODO: this list should be user configurable.
// List of font extensions to search when path contains the '.*' extension.
constexpr const std::array<const char*, 5> kFontExtensions{{
    ".woff",
    ".ttf",
    ".otf",
    ".ttc",
    ".otc",
}};

FontManager::FontManager(FontDriver* fontDriver) noexcept : fontDriver(fontDriver) {
}

void FontManager::SetDefaultFontFamily(std::string&& family) {
  this->defaultFontFamily = family;
}

int32_t FontManager::CreateFont(std::string&& family, int32_t style, int32_t weight) {
  if (!this->fontDriver || this->destroyed) {
    return 0;
  }

  FontStyle styleValue;

  if (IsEnum<FontStyle>(style)) {
    styleValue = static_cast<FontStyle>(style);
  } else {
    styleValue = {};
  }

  FontWeight weightValue;

  if (IsEnum<FontWeight>(weight)) {
    weightValue = static_cast<FontWeight>(weight);
  } else {
    weightValue = {};
  }

  bool exists{};

  for (const auto& p : this->fonts) {
    if (p.second->GetFamily() == family && p.second->GetStyle() == styleValue && p.second->GetWeight() == weightValue) {
      exists = true;
      break;
    }
  }

  if (exists) {
    return 0;
  }

  auto id{ nextResourceId++ };

  this->fonts[id] = std::make_unique<Font>(std::move(family), styleValue, weightValue);
  this->fontTable[this->fonts[id]->GetFamily()][weightValue][styleValue] = this->fonts[id].get();

  return id;
}

FontSource* FontManager::CreateFontSource(const char* file, int32_t index) {
  std::error_code errorCode;

  // Handle the '.*' extension search recursively.
  if (EndsWith(file, ".*")) {
    std17::filesystem::path path = file;

    for (auto& ext : kFontExtensions) {
      path.replace_extension(ext);

      if (!std17::filesystem::exists(path, errorCode)) {
        continue;
      }

      return this->fontDriver->LoadFontSource(path.c_str(), index);
    }

    return nullptr;
  }

  return this->fontDriver->LoadFontSource(file, index);
}

Font* FontManager::GetFont(int32_t id) const noexcept {
  return this->fonts.contains(id) ? this->fonts[id].get() : nullptr;
}

void FontManager::Destroy() {
  if (this->destroyed) {
    return;
  }

  if (this->fontDriver) {
    for (auto& p : this->fonts) {
      this->fontDriver->DestroyFontSource(p.second->GetFontSource());
    }
  }

  this->defaultFontFamily.clear();
  this->fonts.clear();
  this->fontTable.clear();

  this->destroyed = true;
}

Font* FontManager::FindFont(const std::string& family, int32_t style, int32_t weight) noexcept {
  auto font = this->FindFontInternal(family, style, weight);

  return font ? font : this->FindFontInternal(this->defaultFontFamily, style, weight);
}

Font* FontManager::FindFontInternal(const std::string& family, int32_t style, int32_t weight) noexcept {
  if (family.empty() || !this->fontTable.contains(family)
      || !IsEnum<FontStyle>(style) || !IsEnum<FontWeight>(weight)) {
    return {};
  }

  const auto& familyTable = this->fontTable[family];
  auto font = familyTable[style][weight];

  if (font) {
    return font;
  }

  for (int32_t s = 0; s < Count<FontStyle>(); s++) {
    for (int32_t w = 0; w < Count<FontWeight>(); w++) {
      font = familyTable[s][w];

      if (font) {
        return font;
      }
    }
  }

  return {};
}

} // namespace lse
