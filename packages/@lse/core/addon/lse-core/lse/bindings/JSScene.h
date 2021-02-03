/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>

namespace lse {
namespace bindings {

/**
 * Bindings for a javascript Scene object.
 */
class JSScene {
 public:
  static Napi::Value New(const Napi::Env& env,
                         const Napi::Value& stage,
                         const Napi::Value& context);
};

} // namespace bindings
} // namespace lse
