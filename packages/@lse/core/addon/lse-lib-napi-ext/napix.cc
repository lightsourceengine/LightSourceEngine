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

namespace napix {

napi_value get_this(napi_env env, napi_callback_info info) noexcept {
  size_t argc{};
  napi_value self{};

  if (napi_get_cb_info(env, info, &argc, nullptr, &self, nullptr) == napi_ok) {
    return self;
  }

  return {};
}

void* unwrap_this_as(napi_env env, napi_callback_info info) noexcept {
  napi_value self{get_this(env, info)};
  void* unwrapped;

  if (napi_unwrap(env, self, &unwrapped) == napi_ok) {
    return unwrapped;
  }

  return {};
}

void* get_data_raw(napi_env env, napi_callback_info info) noexcept {
  size_t argc{};
  void* data{};

  if (napi_get_cb_info(env, info, &argc, nullptr, nullptr, &data) == napi_ok) {
    return data;
  }

  return {};
}

napi_value to_value_or_null(napi_env env, int32_t value) noexcept {
  napi_value v;
  return napi_create_int32(env, value, &v) == napi_ok ? v : nullptr;
}

napi_value to_value_or_null(napi_env env, bool value) noexcept {
  napi_value v;
  return napi_get_boolean(env, value, &v) == napi_ok ? v : nullptr;
}

napi_value to_value_or_null(napi_env env, float value) noexcept {
  napi_value v;
  return napi_create_double(env, value, &v) == napi_ok ? v : nullptr;
}

napi_value to_value_or_null(napi_env env, const char* value) noexcept {
  napi_value v;

  if (!value) {
    value = "";
  }

  return napi_create_string_utf8(env, value, NAPI_AUTO_LENGTH, &v) == napi_ok ? v : nullptr;
}

napi_value to_value(napi_env env, int32_t value) noexcept {
  napi_value v;
  napi_status status = napi_create_int32(env, value, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_int32");
  }

  return v;
}

napi_value to_value(napi_env env, bool value) noexcept {
  napi_value v;
  napi_status status = napi_get_boolean(env, value, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_get_boolean");
  }

  return v;
}

napi_value to_value(napi_env env, float value) noexcept {
  napi_value v;
  napi_status status = napi_create_double(env, value, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_double");
  }

  return v;
}

napi_value to_value(napi_env env, const char* value) noexcept {
  if (!value) {
    value = "";
  }

  napi_value v;
  napi_status status = napi_create_string_utf8(env, value, NAPI_AUTO_LENGTH, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_string_utf8");
  }

  return v;
}

napi_value to_value(napi_env env, const std::string& value) noexcept {
  return to_value(env, value.c_str());
}

buffer_info as_buffer(napi_env env, napi_value value) noexcept {
  buffer_info bi{};

  if (value) {
    napi_get_buffer_info(env, value, &bi.data, &bi.size);
  }

  return bi;
}

int32_t as_int32(napi_env env, napi_value value, int32_t defaultValue) noexcept {
  int32_t v;

  if (napi_get_value_int32(env, value, &v) != napi_ok) {
    return defaultValue;
  }

  return v;
}

bool as_bool(napi_env env, napi_value value, bool defaultValue) noexcept {
  bool v;

  if (napi_get_value_bool(env, value, &v) != napi_ok) {
    return defaultValue;
  }

  return v;
}

std::string as_string_utf8(napi_env env, napi_value str) noexcept {
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

napi_value new_external(napi_env env, void* data, napi_finalize finalizer) noexcept {
  napi_value value;
  napi_status status;

  status = napi_create_external(env, data, finalizer, nullptr, &value);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_external");
  }

  return value;
}

void throw_error(napi_env env, const char* message) noexcept {
  if (!has_pending_exception(env)) {
    napi_throw_error(env, "", message);
  }
}

bool has_pending_exception(napi_env env) noexcept {
  bool result{};

  return (napi_is_exception_pending(env, &result) != napi_ok) || result;
}

napi_value object_new(napi_env env, const std::initializer_list<napi_property_descriptor>& props) noexcept {
  napi_value obj{};

  if (napi_create_object(env, &obj) != napi_ok) {
    // TODO: throw
    return {};
  }

  if (napi_define_properties(env, obj, props.size(), props.begin()) != napi_ok) {
    // TODO: throw
    return {};
  }

  return obj;
}

int32_t object_get_or(napi_env env, napi_value value, const char* prop, int32_t defaultValue) noexcept {
  napi_value v{};

  napi_get_named_property(env, value, prop, &v);

  return as_int32(env, v, defaultValue);
}

bool object_get_or(napi_env env, napi_value value, const char* prop, bool defaultValue) noexcept {
  napi_value v{};

  napi_get_named_property(env, value, prop, &v);

  return as_bool(env, v, defaultValue);
}

napi_value array_new(napi_env env, size_t length) noexcept {
  napi_value array{};
  napi_status status;

  if (length == 0) {
    status = napi_create_array(env, &array);
  } else {
    status = napi_create_array_with_length(env, length, &array);
  }

  return status == napi_ok ? array : nullptr;
}

bool is_nullish(napi_env env, napi_value value) noexcept {
  napi_valuetype type{};

  if (value) {
    napi_typeof(env, value, &type);
  }

  return type == napi_undefined || type == napi_null;
}

bool is_function(napi_env env, napi_value value) noexcept {
  napi_valuetype type{};
  return (napi_typeof(env, value, &type) == napi_ok && type == napi_function);
}

namespace js_class {

napi_value define(
    napi_env env,
    const char* name,
    napi_callback constructor,
    const std::initializer_list<napi_property_descriptor>& props) noexcept {
  return define(env, name, constructor, props.size(), props.begin());
}

napi_value define(
    napi_env env,
    const char* name,
    napi_callback constructor,
    size_t propCount,
    const napi_property_descriptor* props) noexcept {
  napi_value value{};
  napi_status status = napi_define_class(
      env,
      name,
      NAPI_AUTO_LENGTH,
      constructor,
      nullptr,
      propCount,
      props,
      &value);

  if (status != napi_ok) {
    throw_error(env, "Failed to define class.");
  }

  return value;
}

napi_value constructor_helper(napi_env env,
                              napi_callback_info info,
                              create_native_callback create,
                              napi_finalize finalizer) noexcept {
  if (has_pending_exception(env)) {
    return {};
  }

  napi_value newTarget{};

  if (napi_get_new_target(env, info, &newTarget) != napi_ok || !newTarget) {
    // TODO: THROW!
    return {};
  }

  napi_value thisObj = get_this(env, info);

  if (!thisObj) {
    // TODO: THROW!
    return {};
  }

  auto native{ create(env, info) };

  if (!native || has_pending_exception(env)) {
    if (native) {
      finalizer(env, native, nullptr);
    }

    // TODO: THROW!
    return {};
  }

  auto status = napi_wrap(
      env,
      thisObj,
      native,
      finalizer,
      nullptr,
      nullptr);

  if (status != napi_ok) {
    finalizer(env, native, nullptr);
    // THROW!
    return {};
  }

  return thisObj;
}

} // namespace js_class

namespace descriptor {

napi_property_descriptor instance_method(const name& name, napi_callback method) noexcept {
  return {
      name.utf8,
      name.value,
      method,
      nullptr,
      nullptr,
      nullptr,
      napi_default,
      nullptr
  };
}

napi_property_descriptor instance_accessor(
    const name& name,
    napi_callback getter,
    napi_callback setter,
    napi_property_attributes attr) noexcept {
  return {
      name.utf8,
      name.value,
      nullptr,
      getter,
      setter,
      nullptr,
      attr,
      nullptr
  };
}

napi_property_descriptor instance_accessor(
    const name& name,
    napi_callback getter,
    napi_callback setter,
    intptr_t data,
    napi_property_attributes attr) noexcept {
  return {
      name.utf8,
      name.value,
      nullptr,
      getter,
      setter,
      nullptr,
      attr,
      reinterpret_cast<void*>(data)
  };
}

napi_property_descriptor instance_value(
    napi_env env, const name& name, int32_t value, napi_property_attributes attr) noexcept {
  return {
      name.utf8,
      name.value,
      nullptr,
      nullptr,
      nullptr,
      to_value(env, value),
      attr,
      nullptr
  };
}

napi_property_descriptor instance_value(
    napi_env env, const name& name, const char* value, napi_property_attributes attr) noexcept {
  return {
      name.utf8,
      name.value,
      nullptr,
      nullptr,
      nullptr,
      to_value(env, value),
      attr,
      nullptr
  };
}

napi_property_descriptor instance_value(
    const name& name, napi_value value, napi_property_attributes attr) noexcept {
  return {
      name.utf8,
      name.value,
      nullptr,
      nullptr,
      nullptr,
      value,
      attr,
      nullptr
  };
}

} // namespace descriptor
} // namespace napix
