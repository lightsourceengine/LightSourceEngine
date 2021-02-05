/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "JSFontManager.h"

#include <lse/FontManager.h>
#include <ObjectBuilder.h>
#include <napix.h>

namespace lse {
namespace bindings {

static napi_value SetDefaultFontFamily(napi_env env, napi_callback_info info) noexcept {
  auto call{ napix::get_callback_info<1>(env, info) };

  call.data_as<FontManager>()->SetDefaultFontFamily(napix::as_string_utf8(env, call[0]));

  return {};
}

static napi_value CreateFont(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<3>(env, info) };
  auto family{ napix::as_string_utf8(env, ci[0]) };
  auto style{ napix::as_int32(env, ci[1], 0) };
  auto weight{ napix::as_int32(env, ci[2], 0) };

  auto id{ ci.data_as<FontManager>()->CreateFont(std::move(family), style, weight) };

  return napix::to_value(env, id);
}

static napi_value SetBuffer(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<3>(env, info) };
  auto id{ napix::as_int32(env, ci[0], -1) };
  auto buffer{ napix::as_buffer(env, ci[1]) };
  auto index{ napix::as_int32(env, ci[2], 0) };

  ci.data_as<FontManager>()->SetBuffer(id, buffer.data, buffer.size, index);

  return {};
}

static napi_value SetError(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };
  auto id{ napix::as_int32(env, ci[0], 0) };

  ci.data_as<FontManager>()->SetError(id);

  return {};
}

static napi_value GetStatus(napi_env env, napi_callback_info info) noexcept {
  auto ci{ napix::get_callback_info<1>(env, info) };
  auto id{ napix::as_int32(env, ci[0], 0) };

  return napix::to_value(env, ci.data_as<FontManager>()->GetStatus(id));
}

static napi_value Destroy(napi_env env, napi_callback_info info) noexcept {
  napix::get_data<FontManager>(env, info)->Destroy();

  return {};
}

Napi::Value JSFontManager::New(const Napi::Env& env, std::unique_ptr<FontDriver>&& fontDriver) {
  auto fontManager = std::make_shared<FontManager>(std::move(fontDriver));

  return Napi::ObjectBuilder(env, fontManager.get())
    .WithValue("$native", napix::new_external_shared(env, fontManager))
    .WithMethod("setDefaultFontFamily", &SetDefaultFontFamily)
    .WithMethod("createFont", &CreateFont)
    .WithMethod("setBuffer", &SetBuffer)
    .WithMethod("setError", &SetError)
    .WithMethod("getStatus", &GetStatus)
    .WithMethod("destroy", &Destroy)
    .Freeze()
    .ToObject();
}

} // namespace bindings
} // namespace lse
