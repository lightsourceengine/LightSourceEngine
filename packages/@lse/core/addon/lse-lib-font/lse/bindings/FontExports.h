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

/**
 * Javascript bindings for Font related enums.
 */

napi_value NewFontStatusEnum(napi_env env);
napi_value NewFontStyleEnum(napi_env env);
napi_value NewFontWeightEnum(napi_env env);

/**
 * FontManager javascript bindings. This is a class that sits between the javaascript and
 * native FontManagers.
 *
 * The instantiation of this class depends on the FontDriver app data key to be set in the Habitat.
 */
class CFontManager {
 public:
  static constexpr auto NAME = "CFontManager";
  static constexpr auto CLASS_ID = Habitat::Class::CFontManager;

  static napi_value CreateClass(napi_env env) noexcept;
};

} // namespace bindings
} // namespace lse
