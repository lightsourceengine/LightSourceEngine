/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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

  // Methods used mostly by javascript

  int32_t CreateFont(std::string&& family, int32_t style, int32_t weight);
  void SetDefaultFontFamily(std::string&& family);
  void SetBuffer(int32_t id, void* buffer, size_t bufferSize, int32_t index);
  void SetError(int32_t id);
  FontStatus GetStatus(int32_t id);
  Font* GetFont(int32_t id) const noexcept;
  void Destroy();

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
