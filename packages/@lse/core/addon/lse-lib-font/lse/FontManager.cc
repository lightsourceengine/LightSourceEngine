/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "FontManager.h"

namespace lse {

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

void FontManager::SetBuffer(int32_t id, void* buffer, size_t bufferSize, int32_t index) {
  if (!this->fonts.contains(id)) {
    return;
  }

  FontStatus status;
  auto fontSource{ this->fontDriver->LoadFontSource(buffer, bufferSize, index) };

  if (fontSource) {
    status = FontStatusReady;
  } else {
    status = FontStatusError;
  }

  this->fonts[id]->Update(status, fontSource);
}

void FontManager::SetError(int32_t id) {
  if (!this->fonts.contains(id)) {
    return;
  }

  this->fonts[id]->Update(FontStatusError, nullptr);
}

FontStatus FontManager::GetStatus(int32_t id) {
  if (this->fonts.contains(id)) {
    return this->fonts[id]->GetFontStatus();
  }

  return FontStatusError;
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
  // TODO: style/weight range
  if (!this->fontTable.contains(family)) {
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
