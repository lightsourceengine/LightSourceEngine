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

#include "Font.h"

#include <lse/Log.h>
#include <cassert>

namespace lse {

Font::Font(std::string&& family, FontStyle style, FontWeight weight) noexcept
: family(std::move(family)), style(style), weight(weight) {
}

void Font::AddListener(void* listener, Font::ListenerCallback callback) noexcept {
  if (!listener || !callback) {
    return;
  }

  this->callbacks[listener] = callback;
}

void Font::RemoveListener(void* listener) noexcept {
  if (!listener) {
    return;
  }

  this->callbacks.erase(listener);
}

void Font::SetLoading() noexcept {
  assert(this->status == FontStatusInit);
  this->status = FontStatusLoading;
}

void Font::SetFontSource(FontSource* fontSource) noexcept {
  assert(this->status == FontStatusInit || this->status == FontStatusLoading);

  this->status = fontSource ? FontStatusReady : FontStatusError;
  this->fontSource = fontSource;

  for (const auto& p : this->callbacks) {
    p.second(this, p.first, this->status);
  }

  this->callbacks.clear();
}

} // namespace lse
