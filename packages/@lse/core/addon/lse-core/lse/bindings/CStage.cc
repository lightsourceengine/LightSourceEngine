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

#include "CoreClasses.h"

#include <napix.h>
#include <lse/Stage.h>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

static napi_value Constructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      [](napi_env env, napi_callback_info info) -> void* {
        return new Stage();
      },
      [](napi_env env, void* data, void* hint) {
        static_cast<Stage*>(data)->Unref();
      });
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  unwrap_this_as<Stage>(env, info)->Destroy();
  return {};
}

napi_value CStage::CreateClass(napi_env env) {
  return define(env, NAME, Constructor, {
      instance_method("destroy", &Destroy),
  });
}

} // namespace bindings
} // namespace lse
