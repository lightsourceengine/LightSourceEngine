/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napix.h>
#include <lse/Habitat.h>

namespace lse {
namespace bindings {

template<typename T>
napi_value CSceneNodeConstructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      [](napi_env env, napi_callback_info info) -> void* {
        auto ci{napix::get_callback_info<1>(env, info)};
        Scene* scene{};

        if (Habitat::InstanceOf(env, ci[0], Habitat::Class::CScene)) {
          scene = napix::unwrap_as<Scene>(env, ci[0]);
        }

        if (!scene) {
          napix::throw_error(env, "expected CScene arg");
          return {};
        }

        return new (std::nothrow) T(scene);
      },
      [](napi_env env, void* data, void* hint) {
        static_cast<T*>(data)->Unref();
      });
}

} // namespace bindings
} // namespace lse

