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
#include <lse/RefGraphicsContext.h>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

static void* CreateNative(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };

  return new RefGraphicsContext({
      napix::object_get_or(env, ci[0], "width", 0),
      napix::object_get_or(env, ci[0], "height", 0),
      napix::object_get_or(env, ci[0], "displayIndex", 0),
      napix::object_get_or(env, ci[0], "fullscreen", false)
  });
}

static napi_value Constructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      &CreateNative,
      [](napi_env env, void* data, void* hint) {
        static_cast<RefGraphicsContext*>(data)->Unref();
      });
}

static napi_value GetWidth(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<RefGraphicsContext>(env, info)->GetWidth());
}

static napi_value GetHeight(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<RefGraphicsContext>(env, info)->GetHeight());
}

static napi_value GetTitle(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<RefGraphicsContext>(env, info)->GetTitle());
}

static napi_value GetDisplayIndex(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<RefGraphicsContext>(env, info)->GetDisplayIndex());
}

static napi_value GetRefreshRate(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<RefGraphicsContext>(env, info)->GetRefreshRate());
}

static napi_value IsFullscreen(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<RefGraphicsContext>(env, info)->IsFullscreen());
}

static napi_value SetTitle(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };

  ci.unwrap_this_as<RefGraphicsContext>(env)->SetTitle(napix::as_string_utf8(env, ci[0]).c_str());

  return {};
}

napi_value CRefGraphicsContext::CreateClass(napi_env env) {
  return define(env, NAME, Constructor, {
      instance_method("getWidth", &GetWidth),
      instance_method("getHeight", &GetHeight),
      instance_method("getTitle", &GetTitle),
      instance_method("getDisplayIndex", &GetDisplayIndex),
      instance_method("getRefreshRate", &GetRefreshRate),
      instance_method("isFullscreen", &IsFullscreen),
      instance_method("setTitle", &SetTitle),
  });
}

} // namespace bindings
} // namespace lse
