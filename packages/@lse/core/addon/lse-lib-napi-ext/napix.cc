/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "napix.h"

#include <cstring>
#include <cstdio>

static napi_value throw_error(napi_env env, napi_status status, const char* message) noexcept {
  char code[8];

  snprintf(code, sizeof(code) / sizeof(char), "%i", status);
  napi_throw_error(env, code, message);

  return {};
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

napi_value napix::to_value(napi_env env, int32_t value) noexcept {
  napi_value v;
  napi_status status = napi_create_int32(env, value, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_int32");
  }

  return v;
}

napi_value napix::to_value(napi_env env, bool value) noexcept {
  napi_value v;
  napi_status status = napi_get_boolean(env, value, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_get_boolean");
  }

  return v;
}

napi_value napix::to_value(napi_env env, float value) noexcept {
  napi_value v;
  napi_status status = napi_create_double(env, value, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_double");
  }

  return v;
}

napi_value napix::to_value(napi_env env, const char* value) noexcept {
  if (!value) {
    value = "";
  }

  napi_value v;
  napi_status status = napi_create_string_utf8(env, value, strlen(value), &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_string_utf8");
  }

  return v;
}

napix::buffer_info napix::as_buffer(napi_env env, napi_value value) noexcept {
  napix::buffer_info bi;

  if (value) {
    napi_get_buffer_info(env, value, &bi.data, &bi.size);
  }

  return bi;
}

int32_t napix::as_int32(napi_env env, napi_value value, int32_t defaultValue) noexcept {
  int32_t v;

  if (napi_get_value_int32(env, value, &v) != napi_ok) {
    return defaultValue;
  }

  return v;
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
    return throw_error(env, status, "napi_create_external");
  }

  return value;
}
