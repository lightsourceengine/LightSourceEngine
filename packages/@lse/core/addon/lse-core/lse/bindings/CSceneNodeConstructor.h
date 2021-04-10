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

