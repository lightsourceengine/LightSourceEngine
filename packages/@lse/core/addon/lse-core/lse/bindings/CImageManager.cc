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
#include <lse/Image.h>
#include <lse/ImageManager.h>

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
        return new ImageManager([env](Image* image) {
          auto work = napix::create_async_work(
              env,
              "image load",
              image,
              [](napi_env env, void* data) noexcept {
                static_cast<Image*>(data)->Unref();
              },
              [](void* data) noexcept {
                static_cast<Image*>(data)->OnLoadImageAsync();
              },
              [](napi_env env, bool cancelled, void* data) noexcept {
                static_cast<Image*>(data)->OnLoadImageAsyncComplete();
              });

          // TODO: error checking
          napix::queue_async_work(env, work);

          image->Ref();
        });
      },
      [](napi_env env, void* data, void* hint) {
        static_cast<ImageManager*>(data)->Unref();
      });
}

napi_value CImageManager::CreateClass(napi_env env) noexcept {
  return define(env, NAME, Constructor, {});
}

} // namespace bindings
} // namespace lse
