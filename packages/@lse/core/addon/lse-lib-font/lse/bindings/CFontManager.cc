/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CFontManager.h"

#include <lse/FontManager.h>
#include <napix.h>

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
        auto fontDriver{ Habitat::GetAppDataAs<FontDriver>(env, FontDriver::APP_DATA_KEY) };
        NAPIX_EXPECT_NOT_NULL(env, fontDriver, "No FontDriver installed.", {});
        return new FontManager(fontDriver);
      },
      [](napi_env env, void* data, void* hint) {
        static_cast<FontManager*>(data)->Unref();
      });
}

static napi_value SetDefaultFontFamily(napi_env env, napi_callback_info info) noexcept {
  auto call{ napix::get_callback_info<1>(env, info) };

  call.unwrap_this_as<FontManager>(env)->SetDefaultFontFamily(napix::as_string_utf8(env, call[0]));

  return {};
}

static napi_value CreateFont(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<3>(env, info) };
  auto family{ napix::as_string_utf8(env, ci[0]) };
  auto style{ napix::as_int32(env, ci[1], 0) };
  auto weight{ napix::as_int32(env, ci[2], 0) };

  auto id{ ci.unwrap_this_as<FontManager>(env)->CreateFont(std::move(family), style, weight) };

  return napix::to_value(env, id);
}

static napi_value SetBuffer(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<3>(env, info) };
  auto id{ napix::as_int32(env, ci[0], -1) };
  auto buffer{ napix::as_buffer(env, ci[1]) };
  auto index{ napix::as_int32(env, ci[2], 0) };

  ci.unwrap_this_as<FontManager>(env)->SetBuffer(id, buffer.data, buffer.size, index);

  return {};
}

static napi_value SetError(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };
  auto id{ napix::as_int32(env, ci[0], 0) };

  ci.unwrap_this_as<FontManager>(env)->SetError(id);

  return {};
}

static napi_value GetStatus(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };
  auto id{ napix::as_int32(env, ci[0], 0) };

  return napix::to_value(env, ci.unwrap_this_as<FontManager>(env)->GetStatus(id));
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  napix::unwrap_this_as<FontManager>(env, info)->Destroy();

  return {};
}

napi_value CFontManager::CreateClass(napi_env env) noexcept {
  return define(env, NAME, Constructor, {
      instance_method("setDefaultFontFamily", &SetDefaultFontFamily),
      instance_method("createFont", &CreateFont),
      instance_method("setBuffer", &SetBuffer),
      instance_method("setError", &SetError),
      instance_method("getStatus", &GetStatus),
      instance_method("destroy", &Destroy)
  });
}

} // namespace bindings
} // namespace lse
