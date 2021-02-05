/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "JSFontEnums.h"

#include <lse/Font.h>
#include <ObjectBuilder.h>
#include <napix.h>

namespace lse {
namespace bindings {

Napi::Value NewFontStatusEnum(const Napi::Env& env) {
  auto attributes{ napi_enumerable };

  return Napi::ObjectBuilder(env)
    .WithValue("INIT", napix::to_value(env, FontStatusInit), attributes)
    .WithValue("LOADING", napix::to_value(env, FontStatusLoading), attributes)
    .WithValue("READY", napix::to_value(env, FontStatusReady), attributes)
    .WithValue("ERROR", napix::to_value(env, FontStatusError), attributes)
    .Freeze()
    .ToObject();
}

Napi::Value NewFontStyleEnum(const Napi::Env& env) {
  auto attributes{ napi_enumerable };

  return Napi::ObjectBuilder(env)
    .WithValue("NORMAL", napix::to_value(env, FontStyleNormal), attributes)
    .WithValue("ITALIC", napix::to_value(env, FontStyleItalic), attributes)
    .WithValue("OBLIQUE", napix::to_value(env, FontStyleOblique), attributes)
    .Freeze()
    .ToObject();
}

Napi::Value NewFontWeightEnum(const Napi::Env& env) {
  auto attributes{ napi_enumerable };

  return Napi::ObjectBuilder(env)
    .WithValue("NORMAL", napix::to_value(env, FontWeightNormal), attributes)
    .WithValue("BOLD", napix::to_value(env, FontWeightBold), attributes)
    .Freeze()
    .ToObject();
}

} // namespace bindings
} // namespace lse
