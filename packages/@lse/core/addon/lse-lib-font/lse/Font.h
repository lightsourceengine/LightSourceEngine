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
  T* GetFontSourceAs() const noexcept { return static_cast<T*>(this->fontSource); }
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
