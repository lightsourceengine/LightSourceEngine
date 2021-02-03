/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace lse {
namespace bindings {

/**
 * Bindings for a javascript Stage object.
 */
class JSStage {
 public:
  static Napi::Value New(const Napi::Env& env);
};

} // namespace bindings
} // namespace lse
