/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/FontDriver.h>

namespace lse {
namespace bindings {

/**
 * Javascript bindings for FontManager.
 */
class JSFontManager {
 public:
  /**
   * Create a new FontManager object.
   */
  static Napi::Value New(const Napi::Env& env, std::unique_ptr<FontDriver>&& fontDriver);
};

} // namespace bindings
} // namespace lse
