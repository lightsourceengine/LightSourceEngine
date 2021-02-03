/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "napix.h"

#include <cstring>
// TODO: remove, used for Napi::Error
#include <napi.h>

void napix::get_args(napi_env env, napi_callback_info info, napi_value* arg) noexcept {
  size_t argc{1};

  if (napi_get_cb_info(env, info, &argc, arg, nullptr, nullptr) != napi_ok) {
    *arg = {};
  }
}

void* napix::get_data_raw(napi_env env, napi_callback_info info) noexcept {
  size_t argc{};
  void* data{};

  if (napi_get_cb_info(env, info, &argc, nullptr, nullptr, &data) == napi_ok) {
    return data;
  }

  return {};
}

napi_value napix::to_value_or_null(napi_env env, int32_t value) noexcept {
  napi_value v;
  return napi_create_int32(env, value, &v) == napi_ok ? v : nullptr;
}

napi_value napix::to_value_or_null(napi_env env, bool value) noexcept {
  napi_value v;
  return napi_get_boolean(env, value, &v) == napi_ok ? v : nullptr;
}

napi_value napix::to_value_or_null(napi_env env, float value) noexcept {
  napi_value v;
  return napi_create_double(env, value, &v) == napi_ok ? v : nullptr;
}

napi_value napix::to_value_or_null(napi_env env, const char* value) noexcept {
  napi_value v;

  if (!value) {
    value = "";
  }

  return napi_create_string_utf8(env, value, strlen(value), &v) == napi_ok ? v : nullptr;
}

std::string napix::as_string_utf8(napi_env env, napi_value str) noexcept {
  if (!str) {
    return {};
  }

  size_t len;
  auto status = napi_get_value_string_utf8(env, str, nullptr, 0, &len);

  if (status != napi_ok) {
    return {};
  }

  std::string value;

  value.reserve(len + 1);
  value.resize(len);

  if (napi_get_value_string_utf8(env, str, &value[0], value.capacity(), nullptr) != napi_ok) {
    return {};
  }

  return value;
}

napi_value napix::new_external(napi_env env, void* data, napi_finalize finalizer) noexcept {
  napi_value value;
  napi_status status;

  status = napi_create_external(env, data, finalizer, nullptr, &value);

  if (status != napi_ok) {
    Napi::Error::New(env).ThrowAsJavaScriptException();
    return {};
  }

  return value;
}
