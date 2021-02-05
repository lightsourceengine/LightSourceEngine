/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "Font.h"

#include <lse/Log.h>

namespace lse {

Font::Font(std::string&& family, FontStyle style, FontWeight weight) noexcept
: family(std::move(family)), style(style), weight(weight) {
}

void Font::AddListener(void* listener, Font::ListenerCallback callback) noexcept {
  if (!listener || !callback) {
    LOG_ERROR("listener is null")
    return;
  }

  this->callbacks[listener] = callback;
}

void Font::RemoveListener(void* listener) noexcept {
  if (!listener) {
    LOG_ERROR("listener is null")
    return;
  }

  this->callbacks.erase(listener);
}

void Font::Update(FontStatus status, FontSource* fontSource) noexcept {
  if (this->status == FontStatusReady || this->status == FontStatusError) {
    LOG_ERROR("font terminal state has already been set");
    return;
  }

  this->status = status;
  this->fontSource = fontSource;

  for (const auto& p : this->callbacks) {
    p.second(this, p.first, this->status);
  }

  this->callbacks.clear();
}

} // namespace lse
