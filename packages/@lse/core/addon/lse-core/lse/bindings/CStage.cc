/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
