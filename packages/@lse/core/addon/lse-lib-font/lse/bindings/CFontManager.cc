/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "CFontManager.h"

#include <lse/FontManager.h>
#include <napix.h>
#include <lse/Log.h>

using napix::unwrap_this_as;
using napix::js_class::define;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

struct FontLoadContext {
  ~FontLoadContext();

  napi_env env{};
  int32_t id{};
  std::string file{};
  int32_t index{};
  ReferenceHolder<FontManager> fontManager{};
  napi_ref callbackRef{};
  FontSource* fontSource{};
};

static void OnFontLoadComplete(napi_env env, bool cancelled, void* data) noexcept;
static void OnFontLoad(void* data) noexcept;

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

static napi_value LoadFontFromFile(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<4>(env, info)};
  auto fontManager{ci.unwrap_this_as<FontManager>(env)};
  auto id{napix::as_int32(env, ci[0], 0)};
  auto file{napix::as_string_utf8(env, ci[1])};
  auto index{napix::as_int32(env, ci[2], 0)};
  auto callback{ci[3]};
  auto font{fontManager->GetFont(id)};

  NAPIX_EXPECT_NOT_NULL(env, font, "Invalid font id", {})
  NAPIX_EXPECT_TRUE(env, napix::is_function(env, ci[3]), "callback required", {})
  NAPIX_EXPECT_FALSE(env, file.empty(), "filename required", {})

  napi_ref callbackRef{};
  if (napi_create_reference(env, callback, 1, &callbackRef) != napi_ok) {
    napix::throw_error(env, "failed to create callback ref");
    return {};
  }

  auto context = new (std::nothrow) FontLoadContext{ env, id, file, index, { fontManager }, callbackRef };

  if (!context) {
    napi_delete_reference(env, callbackRef);
    napix::throw_error(env, "failed to create native context");
    return {};
  }

  auto work = napix::create_async_work(
      env,
      "font loader",
      context,
      &napix::finalize_impl<FontLoadContext>,
      OnFontLoad,
      OnFontLoadComplete);

  if (work) {
    if (napix::queue_async_work(env, work) != napi_ok) {
      napix::cancel_async_work(env, work);
      font->SetFontSource(nullptr);
      napix::throw_error(env, "failed to queue work");
      return {};
    } else {
      font->SetLoading();
    }
  } else {
    font->SetFontSource(nullptr);
    napix::throw_error(env, "failed to create work");
    return {};
  }

  return napix::to_value(env, font->GetFontStatus());
}

static napi_value LoadFontFromBuffer(napi_env env, napi_callback_info info) noexcept {
  auto ci{napix::get_callback_info<3>(env, info)};
  auto fontManager{ci.unwrap_this_as<FontManager>(env)};
  auto id{ napix::as_int32(env, ci[0], 0) };
  auto buffer{ napix::as_buffer(env, ci[1]) };
  auto index{ napix::as_int32(env, ci[2], 0) };
  auto font{fontManager->GetFont(id)};

  NAPIX_EXPECT_NOT_NULL(env, font, "Invalid font id", {})

  FontSource* fontSource{};

  if (!buffer.empty()) {
    // The memory pointer of buffer is managed by node. The pointer is not stable, nor thread safe,
    // due to gc reorganizing memory. LoadFontSource will make a copy of this buffer and manage
    // the memory accordingly.
    fontSource = fontManager->GetFontDriver()->LoadFontSource(buffer.data, buffer.size, index);
  }

  font->SetFontSource(fontSource);

  return napix::to_value(env, font->GetFontStatus());
}

static napi_value GetStatus(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };
  auto id{ napix::as_int32(env, ci[0], 0) };
  auto fontManager{ci.unwrap_this_as<FontManager>(env)};
  auto font{fontManager->GetFont(id)};

  return napix::to_value(env, font ? font->GetFontStatus() : FontStatusError);
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  napix::unwrap_this_as<FontManager>(env, info)->Destroy();
  return {};
}

FontLoadContext::~FontLoadContext() {
  if (this->callbackRef) {
    napi_delete_reference(this->env, this->callbackRef);
  }
}

static void OnFontLoad(void* data) noexcept {
  auto context{static_cast<FontLoadContext*>(data)};

  context->fontSource = context->fontManager->CreateFontSource(context->file.c_str(), context->index);
}

static void OnFontLoadComplete(napi_env env, bool cancelled, void* data) noexcept {
  auto context{static_cast<FontLoadContext*>(data)};

  if (cancelled || context->fontManager->IsDestroyed()) {
    LOG_INFO("cancelled");
    return;
  }

  auto font{context->fontManager->GetFont(context->id)};

  if (!font) {
    LOG_INFO("font removed");
    return;
  }

  font->SetFontSource(context->fontSource);

  auto callStatus = napix::call_function(
      env,
      context->callbackRef,
    { napix::to_value(env, context->id), napix::to_value(env, font->GetFontStatus()) },
    nullptr);

  if (callStatus != napi_ok) {
    LOG_ERROR("callback invoke: %i", callStatus);
    if (napix::has_pending_exception(env)) {
      napi_value result{};
      napi_get_and_clear_last_exception(env, &result);
      LOG_ERROR("Uncaught JS exception: %s", napix::as_string_utf8(env, result));
    }
    return;
  }
}

napi_value CFontManager::CreateClass(napi_env env) noexcept {
  return define(env, NAME, Constructor, {
      instance_method("setDefaultFontFamily", &SetDefaultFontFamily),
      instance_method("createFont", &CreateFont),
      instance_method("loadFontFromBuffer", &LoadFontFromBuffer),
      instance_method("loadFontFromFile", &LoadFontFromFile),
      instance_method("getStatus", &GetStatus),
      instance_method("destroy", &Destroy)
  });
}

} // namespace bindings
} // namespace lse
