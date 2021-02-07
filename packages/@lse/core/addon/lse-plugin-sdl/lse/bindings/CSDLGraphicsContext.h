/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/Habitat.h>

namespace lse {
namespace bindings {

class CSDLGraphicsContext {
 public:
  static constexpr auto NAME = "CSDLGraphicsContext";
  static constexpr auto CLASS_ID = Habitat::Class::CGraphicsContext;

  static napi_value CreateClass(napi_env env);
};

} // namespace bindings
} // namespace lse
