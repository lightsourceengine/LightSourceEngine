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

struct _async_work {
  void* magic{};
  void* data{};
  finalize finalizer{};
  on_execute onExecute{};
  on_complete onComplete{};
  napi_async_work work{};
  bool cancelled{};
};

static char asyncWorkMagic{};

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

napi_value to_value_or_null(napi_env env, uint32_t value) noexcept {
  napi_value v;
  return napi_create_uint32(env, value, &v) == napi_ok ? v : nullptr;
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

napi_value to_value(napi_env env, uint32_t value) noexcept {
  napi_value v;
  napi_status status = napi_create_uint32(env, value, &v);

  if (status != napi_ok) {
    return throw_error(env, status, "napi_create_uint32");
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
  bool result{};

  if (value && napi_is_buffer(env, value, &result) == napi_ok && result) {
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

int32_t as_uint32(napi_env env, napi_value value, uint32_t defaultValue) noexcept {
  uint32_t v;

  if (napi_get_value_uint32(env, value, &v) != napi_ok) {
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

float as_float(napi_env env, napi_value value, float defaultValue) noexcept {
  double v;

  if (napi_get_value_double(env, value, &v) != napi_ok) {
    return defaultValue;
  }

  return static_cast<float>(v);
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

const char* copy_utf8(napi_env env, napi_value value, char* buffer, size_t bufferSize, const char* fallback) noexcept {
  if (!value) {
    return fallback;
  }

  size_t length{};
  auto status = napi_get_value_string_utf8(env, value, nullptr, 0, &length);

  if (status != napi_ok || length >= bufferSize) {
    return fallback;
  }

  status = napi_get_value_string_utf8(env, value, buffer, bufferSize, nullptr);

  return status == napi_ok ? buffer : fallback;
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

std::string pop_pending_exception(napi_env env) noexcept {
  napi_value error{};
  if (napi_get_and_clear_last_exception(env, &error) != napi_ok) {
    return {};
  }

  napi_value errorString{};
  if (napi_coerce_to_string(env, error, &errorString) != napi_ok) {
    return {};
  }

  return as_string_utf8(env, errorString);
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

float object_get_or(napi_env env, napi_value value, const char* prop, float defaultValue) noexcept {
  napi_value v{};

  napi_get_named_property(env, value, prop, &v);

  return as_float(env, v, defaultValue);
}

napi_value object_at(napi_env env, napi_value value, uint32_t index) noexcept {
  napi_value v{};

  napi_get_element(env, value, index, &v);

  return v;
}

int32_t object_at_or(napi_env env, napi_value value, uint32_t index, int32_t defaultValue) noexcept {
  return as_int32(env, object_at(env, value, index), defaultValue);
}

uint32_t object_at_or(napi_env env, napi_value value, uint32_t index, uint32_t defaultValue) noexcept {
  return as_uint32(env, object_at(env, value, index), defaultValue);
}

float object_at_or(napi_env env, napi_value value, uint32_t index, float defaultValue) noexcept {
  return as_float(env, object_at(env, value, index), defaultValue);
}

bool object_get_or(napi_env env, napi_value value, const char* prop, bool defaultValue) noexcept {
  napi_value v{};

  napi_get_named_property(env, value, prop, &v);

  return as_bool(env, v, defaultValue);
}

std::string object_get(napi_env env, napi_value value, const char* prop) noexcept {
  napi_valuetype type{};
  napi_typeof(env, value, &type);

  if (type == napi_object) {
    napi_value v{};
    napi_get_named_property(env, value, prop, &v);
    return as_string_utf8(env, v);
  }

  return {};
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

napi_value array_new(napi_env env, const std::initializer_list<napi_value>& values) noexcept {
  auto array{array_new(env, values.size())};
  uint32_t index{0};

  if (!array) {
    return {};
  }

  for (auto& value : values) {
    if (napi_set_element(env, array, index++, value) != napi_ok) {
      return {};
    }
  }

  return array;
}

bool is_nullish(napi_env env, napi_value value) noexcept {
  napi_valuetype type{napi_undefined};

  if (value) {
    napi_typeof(env, value, &type);
  }

  return type == napi_undefined || type == napi_null;
}

bool is_function(napi_env env, napi_value value) noexcept {
  napi_valuetype type{};
  return (napi_typeof(env, value, &type) == napi_ok && type == napi_function);
}

bool is_string(napi_env env, napi_value value) noexcept {
  napi_valuetype type{};
  return (napi_typeof(env, value, &type) == napi_ok && type == napi_string);
}

bool is_number(napi_env env, napi_value value) noexcept {
  napi_valuetype type{};
  return (napi_typeof(env, value, &type) == napi_ok && type == napi_number);
}

bool is_buffer(napi_env env, napi_value value) noexcept {
  bool result{};
  return (napi_is_buffer(env, value, &result) == napi_ok && result);
}

bool is_array(napi_env env, napi_value value) noexcept {
  bool result{};
  return (napi_is_array(env, value, &result) == napi_ok && result);
}

napi_status call_function(
    napi_env env,
    napi_ref functionRef,
    const std::initializer_list<napi_value>& args,
    napi_value* result) noexcept {
  napi_value function{};
  if (napi_get_reference_value(env, functionRef, &function) != napi_ok || !function) {
    return napi_status::napi_function_expected;
  }

  napi_value undefined{};
  napi_get_undefined(env, &undefined);

  if (!undefined) {
    return napi_status::napi_generic_failure;
  }

  return napi_call_function(env, undefined, function, args.size(), args.begin(), result);
}

static bool check_async_work(async_work work) noexcept {
  return (work && work->magic == &asyncWorkMagic);
}

static void async_work_delete(napi_env env, async_work work) noexcept {
  if (check_async_work(work)) {
    if (work->data && work->finalizer) {
      work->finalizer(env, work->data);
    }

    if (work->work) {
      napi_delete_async_work(env, work->work);
    }

    work->magic = nullptr;
    delete work;
  }
}

static void async_work_execute_wrapper(napi_env env, void* data) noexcept {
  auto work{static_cast<async_work>(data)};

  if (check_async_work(work)) {
    work->onExecute(work->data);
  }
}

static void async_work_complete_wrapper(napi_env env, napi_status status, void* data) noexcept {
  auto work{static_cast<async_work>(data)};

  if (!check_async_work(work)) {
    return;
  }

  if (work->onComplete) {
    work->onComplete(env, work->cancelled || status == napi_cancelled, work->data);
  }

  async_work_delete(env, work);
}

async_work create_async_work(
    napi_env env,
    const char* resourceName,
    void* data,
    finalize finalizer,
    on_execute onExecute,
    on_complete onComplete) noexcept {
  napi_value name{to_value_or_null(env, resourceName)};

  if (!name) {
    return {};
  }

  auto state = new (std::nothrow) _async_work{
    &asyncWorkMagic,
    data,
    finalizer,
    onExecute,
    onComplete
  };

  if (!state) {
    if (data && finalizer) {
      finalizer(env, data);
    }
    return {};
  }

  napi_async_work work{};
  auto status = napi_create_async_work(
      env,
      nullptr,
      name,
      async_work_execute_wrapper,
      async_work_complete_wrapper,
      state,
      &work);

  if (status != napi_ok) {
    async_work_delete(env, state);
    return {};
  }

  state->work = work;

  return state;
}

void cancel_async_work(napi_env env, async_work work) noexcept {
  if (work && work->magic == &asyncWorkMagic) {
    auto status = napi_cancel_async_work(env, work->work);

    if (status == napi_ok) {
      async_work_delete(env, work);
    } else if (status == napi_generic_failure) {
      // inflight, complete callback will delete work

      // set the cancelled flag for the case of the complete callback in the
      // microtask queue and napi_status not set.
      work->cancelled = true;
    }
  }
}

napi_status queue_async_work(napi_env env, async_work work) noexcept {
  if (check_async_work(work)) {
    return napi_queue_async_work(env, work->work);
  }

  return napi_invalid_arg;
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
