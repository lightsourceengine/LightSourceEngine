/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "FontExports.h"

#include <lse/Font.h>
#include <napix.h>

using napix::object_new;
using napix::descriptor::instance_value;

namespace lse {
namespace bindings {

napi_value NewFontStatusEnum(napi_env env) {
  return object_new(env, {
      instance_value(env, "INIT", FontStatusInit, napi_enumerable),
      instance_value(env, "LOADING", FontStatusLoading, napi_enumerable),
      instance_value(env, "READY", FontStatusReady, napi_enumerable),
      instance_value(env, "ERROR", FontStatusError, napi_enumerable),
  });
}

napi_value NewFontStyleEnum(napi_env env) {
  return object_new(env, {
      instance_value(env, "NORMAL", FontStyleNormal, napi_enumerable),
      instance_value(env, "ITALIC", FontStyleItalic, napi_enumerable),
      instance_value(env, "OBLIQUE", FontStyleOblique, napi_enumerable),
  });
}

napi_value NewFontWeightEnum(napi_env env) {
  return object_new(env, {
      instance_value(env, "NORMAL", FontWeightNormal, napi_enumerable),
      instance_value(env, "BOLD", FontWeightBold, napi_enumerable),
  });
}

} // namespace bindings
} // namespace lse
