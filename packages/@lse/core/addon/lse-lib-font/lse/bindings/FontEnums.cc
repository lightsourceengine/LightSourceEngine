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
