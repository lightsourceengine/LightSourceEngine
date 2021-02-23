/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
