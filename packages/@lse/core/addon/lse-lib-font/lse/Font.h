/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <string>
#include <phmap.h>
#include <lse/FontDriver.h>
#include <lse/EnumSequence.h>

namespace lse {

/**
 * Represents the current state of a Font resource.
 */
LSE_ENUM_SEQ_DECL(
  FontStatus,
  FontStatusInit,
  FontStatusLoading,
  FontStatusReady,
  FontStatusError
)

// XXX: Redefinition of StyleFontStyle and StyleFontWeight, but those enums are not available to lse-lib-font

LSE_ENUM_SEQ_DECL(
    FontWeight,
    FontWeightNormal,
    FontWeightBold
)

LSE_ENUM_SEQ_DECL(
    FontStyle,
    FontStyleNormal,
    FontStyleItalic,
    FontStyleOblique
)

/**
 * Font resource.
 *
 * - Wrapper around a font face (loaded TTF file)
 * - Used by TextSceneNodes to get the font face. If the font face is loading, the node
 *   can add a listener to wait for it to be in a terminal state (ready or error).
 */
class Font {
 public:
  using ListenerCallback = void(*)(Font*, void*, FontStatus);

 public:
  Font(std::string&& family, FontStyle style, FontWeight weight) noexcept;

  /**
   * Sets the font face and the terminal state of this font (ready or error). This
   * call will notify native listeners of the change during this call.
   *
   * Re-entrant listener calls are not a concern, as Update is only invoked from javascript
   * and the native listeners don't call back into Font or FontManager.
   */
  void SetFontSource(FontSource* fontSource) noexcept;

  /**
   * Puts the font in the loading state. No listeners are notified.
   */
  void SetLoading() noexcept;

  const std::string& GetFamily() const noexcept { return this->family; }
  FontStyle GetStyle() const noexcept { return this->style; }
  FontWeight GetWeight() const noexcept { return this->weight; }
  FontSource* GetFontSource() const noexcept { return this->fontSource; }
  template<typename T>
  T GetFontSourceAs() const noexcept { return static_cast<T>(this->fontSource); }
  FontStatus GetFontStatus() const noexcept { return this->status; }

  void AddListener(void* listener, ListenerCallback callback) noexcept;
  void RemoveListener(void* listener) noexcept;

 private:
  phmap::flat_hash_map<void*, ListenerCallback> callbacks;
  std::string family{};
  FontStyle style{};
  FontWeight weight{};
  FontStatus status{FontStatusInit};
  FontSource* fontSource{};
};

} // namespace lse
