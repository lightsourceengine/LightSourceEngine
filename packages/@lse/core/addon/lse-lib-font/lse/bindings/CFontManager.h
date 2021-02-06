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

class CFontManager {
 public:
  static constexpr auto NAME = "CFontManager";
  static constexpr auto CLASS_ID = Habitat::Class::CFontManager;

  static napi_value CreateClass(napi_env env) noexcept;
};

} // namespace bindings
} // namespace lse
