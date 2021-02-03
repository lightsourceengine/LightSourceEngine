/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "JSGraphicsContext.h"
#include <ObjectBuilder.h>
#include <napix.h>

namespace lse {
namespace bindings {

static napi_value GetWidth(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, napix::get_data<GraphicsContext>(env, info)->GetWidth());
}

static napi_value GetHeight(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, napix::get_data<GraphicsContext>(env, info)->GetHeight());
}

static napi_value GetTitle(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, napix::get_data<GraphicsContext>(env, info)->GetTitle());
}

static napi_value GetDisplayIndex(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, napix::get_data<GraphicsContext>(env, info)->GetDisplayIndex());
}

static napi_value IsFullscreen(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, napix::get_data<GraphicsContext>(env, info)->IsFullscreen());
}

static napi_value SetTitle(napi_env env, napi_callback_info info) noexcept {
  napi_value title;

  napix::get_args(env, info, &title);
  napix::get_data<GraphicsContext>(env, info)->SetTitle(napix::as_string_utf8(env, title).c_str());

  return {};
}

static void SetConfig(GraphicsContextRef& context, const Napi::Value& options) {
  if (!options.IsObject()) {
    return;
  }

  auto obj { options.As<Napi::Object>() };

  context->SetConfig({
     Napi::ObjectGetNumberOrDefault(obj, "width", 0),
     Napi::ObjectGetNumberOrDefault(obj, "height", 0),
     Napi::ObjectGetNumberOrDefault(obj, "displayIndex", 0),
     Napi::ObjectGetBooleanOrDefault(obj, "fullscreen", false)
  });
}

Napi::Value JSGraphicsContext::New(
    const Napi::Env& env, const Napi::Value& options, GraphicsContextRef& context) {
  SetConfig(context, options);

  return Napi::ObjectBuilder(env, context.get())
    .WithValue("$native", napix::new_external_shared(env, context))
    .WithMethod("getWidth", &GetWidth)
    .WithMethod("getHeight", &GetHeight)
    .WithMethod("getTitle", &GetTitle)
    .WithMethod("getDisplayIndex", &GetDisplayIndex)
    .WithMethod("isFullscreen", &IsFullscreen)
    .WithMethod("setTitle", &SetTitle)
    .Freeze()
    .ToObject();
}

} // namespace bindings
} // namespace lse
