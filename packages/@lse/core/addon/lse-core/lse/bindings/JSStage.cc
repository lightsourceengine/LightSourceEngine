/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "JSStage.h"

#include <napix.h>
#include <ObjectBuilder.h>
#include <lse/Stage.h>

namespace lse {
namespace bindings {

static napi_value Update(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(napix::get_data<Stage>(env, info)->Update())
  return {};
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  NAPIX_TRY_STD(napix::get_data<Stage>(env, info)->Destroy())
  return {};
}

Napi::Value JSStage::New(const Napi::Env& env) {
  auto stage = std::make_shared<Stage>();

  return Napi::ObjectBuilder(env, stage.get())
      .WithValue("$native", napix::new_external_shared(env, stage))
      .WithMethod("update", &Update)
      .WithMethod("destroy", &Destroy)
      .Freeze()
      .ToObject();
}

} // namespace bindings
} // namespace lse
