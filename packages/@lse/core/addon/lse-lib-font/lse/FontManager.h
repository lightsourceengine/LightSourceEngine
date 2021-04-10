/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <lse/Font.h>
#include <lse/Reference.h>

namespace lse {

/**
 * Font Manager
 *
 * The native part of the Font Manager stores fonts, provides a font search method for style font
 * look ups and broadcasts font state change events.
 *
 * The javascript part of the Font Manager stores fonts, load fonts from file and drives state
 * changes.
 *
 * The native interface is exposed to javascript as a class. The javascript FontManager uses
 * the native class with composition.
 *
 * The actual loading and parsing of font files (ttf, etc) is delegated to the FontDriver. The driver
 * is installed when the FontManager is created.
 *
 * The FontManager is available on construction. When destroy() is called, the FontManager is cleared
 * and further font creation is disabled. Calls to a destroyed FontManager will not crash the app.
 *
 * The FontManager is owned by the stage, so it effectively a singleton.
 */
class FontManager : public Reference {
 public:
  explicit FontManager(FontDriver* fontDriver) noexcept;

  /**
   * Find a font.
   *
   * Search algorithm steps:
   *
   * - return an exact match of family + style + weight
   * - return another font within the family
   * - return an exact match of default + style + weight
   * - return any other font within the default family
   * - return null
   */
  Font* FindFont(const std::string& family, int32_t style, int32_t weight) noexcept;

  FontDriver* GetFontDriver() const noexcept { return this->fontDriver; }

  // Methods used mostly by javascript

  int32_t AddFont(std::string&& family, int32_t style, int32_t weight);
  FontSource* CreateFontSource(const char* file, int32_t index);
  void SetDefaultFontFamily(std::string&& family);
  Font* GetFont(int32_t id) const noexcept;
  void Destroy();
  bool IsDestroyed() const noexcept { return this->destroyed; }

 private:
  Font* FindFontInternal(const std::string& family, int32_t style, int32_t weight) noexcept;

 private:
  FontDriver* fontDriver;
  int32_t nextResourceId{1};
  mutable phmap::flat_hash_map<int32_t, std::unique_ptr<Font>> fonts{};
  phmap::flat_hash_map<std::string, std::array<std::array<Font*, Count<FontStyle>()>, Count<FontWeight>()>> fontTable{};
  std::string defaultFontFamily{};
  bool destroyed{false};
};

} // namespace lse
