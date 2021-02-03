/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "JSGraphicsContext.h"
#include <ObjectBuilder.h>
#include <cassert>

namespace lse {
namespace bindings {

static GraphicsContext* GetData(napi_env env, napi_callback_info info) noexcept {
  size_t argc{};
  void* data{};

  napi_status status = napi_get_cb_info(env, info, &argc, nullptr, nullptr, &data);

  return status == napi_ok ? static_cast<GraphicsContext*>(data) : nullptr;
}

static void GraphicsContextFinalizer(napi_env env, void* data, void* hint) noexcept {
  delete static_cast<std::shared_ptr<GraphicsContext>*>(data);
}

static napi_value GetWidth(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::NewNumber(env, GetData(env, info)->GetWidth()))
}

static napi_value GetHeight(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::NewNumber(env, GetData(env, info)->GetHeight()))
}

static napi_value GetTitle(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::String::New(env, GetData(env, info)->GetTitle()))
}

static napi_value GetDisplayIndex(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::NewNumber(env, GetData(env, info)->GetDisplayIndex()))
}

static napi_value IsFullscreen(napi_env env, napi_callback_info info) noexcept {
  NAPI_TRY_C(return Napi::Boolean::New(env, GetData(env, info)->IsFullscreen()))
}

static napi_value SetTitle(napi_env env, napi_callback_info info) noexcept {
  try {
    Napi::CallbackInfo ci(env, info);

    static_cast<GraphicsContext*>(ci.Data())->SetTitle(Napi::CopyUtf8(ci[0]));
  } catch (const Napi::Error& e) {
    e.ThrowAsJavaScriptException();
  }
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

static Napi::External<GraphicsContextRef*> CreateExternal(const Napi::Env& env, GraphicsContextRef& context) {
  auto wrapper = new GraphicsContextRef(context);
  napi_value value;
  napi_status status;

  status = napi_create_external(env, wrapper, &GraphicsContextFinalizer, nullptr, &value);
  NAPI_THROW_IF_FAILED(env, status, {});

  return { env, value };
}

Napi::Value JSGraphicsContext::New(
    const Napi::Env& env, const Napi::Value& options, GraphicsContextRef& context) {
  SetConfig(context, options);

  return Napi::ObjectBuilder(env, context.get())
    .WithValue("$native", CreateExternal(env, context))
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
