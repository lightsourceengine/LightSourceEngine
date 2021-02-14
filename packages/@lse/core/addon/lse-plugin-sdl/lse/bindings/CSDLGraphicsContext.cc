/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CSDLGraphicsContext.h"

#include <napix.h>
#include <lse/SDLGraphicsContext.h>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

static void* CreateNative(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };

  return new SDLGraphicsContext({
    napix::object_get_or(env, ci[0], "width", 0),
    napix::object_get_or(env, ci[0], "height", 0),
    napix::object_get_or(env, ci[0], "displayIndex", 0),
    napix::object_get_or(env, ci[0], "fullscreen", false),
    napix::object_get(env, ci[0], "fullscreenMode")
  });
}

static napi_value Constructor(napi_env env, napi_callback_info info) noexcept {
  return napix::js_class::constructor_helper(
      env,
      info,
      &CreateNative,
      [](napi_env env, void* data, void* hint) {
        static_cast<SDLGraphicsContext*>(data)->Unref();
      });
}

static napi_value GetWidth(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<SDLGraphicsContext>(env, info)->GetWidth());
}

static napi_value GetHeight(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<SDLGraphicsContext>(env, info)->GetHeight());
}

static napi_value GetTitle(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<SDLGraphicsContext>(env, info)->GetTitle());
}

static napi_value GetDisplayIndex(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<SDLGraphicsContext>(env, info)->GetDisplayIndex());
}

static napi_value IsFullscreen(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, unwrap_this_as<SDLGraphicsContext>(env, info)->IsFullscreen());
}

static napi_value SetTitle(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };

  ci.unwrap_this_as<SDLGraphicsContext>(env)->SetTitle(napix::as_string_utf8(env, ci[0]).c_str());

  return {};
}

napi_value CSDLGraphicsContext::CreateClass(napi_env env) {
  return define(env, NAME, Constructor, {
      instance_method("getWidth", &GetWidth),
      instance_method("getHeight", &GetHeight),
      instance_method("getTitle", &GetTitle),
      instance_method("getDisplayIndex", &GetDisplayIndex),
      instance_method("isFullscreen", &IsFullscreen),
      instance_method("setTitle", &SetTitle),
  });
}

} // namespace bindings
} // namespace lse
