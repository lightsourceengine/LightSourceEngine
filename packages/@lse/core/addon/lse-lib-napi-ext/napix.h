/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <node_api.h>
#include <string>
#include <memory>
#include <array>
#include <cstdio>

#define NAPIX_TRY_STD(ENV, EXPR, RET) \
  try { \
    (EXPR); \
  } catch (const std::exception& e) { \
    napix::throw_error(ENV, e.what()); \
    return RET;\
  }
// if expr == true, throw Error
#define NAPIX_EXPECT_TRUE(ENV, EXPR, MESSAGE, RET) \
  if (!(EXPR)) { napix::throw_error(env, MESSAGE); return RET; }
// if expr == false, throw Error
#define NAPIX_EXPECT_FALSE(ENV, EXPR, MESSAGE, RET) \
  if (EXPR) { napix::throw_error(env, MESSAGE); return RET; }
// if expr is not null, throw Error
#define NAPIX_EXPECT_NULL(ENV, EXPR, MESSAGE, RET) \
  if ((EXPR) != nullptr) { napix::throw_error(env, MESSAGE); return RET; }
// if expr is null, throw Error
#define NAPIX_EXPECT_NOT_NULL(ENV, EXPR, MESSAGE, RET) \
  if ((EXPR) == nullptr) { napix::throw_error(env, MESSAGE); return RET; }

/**
 * Helper methods for with the napi C api.
 *
 * DEV NOTES
 *
 * The Napi C++ api creates a lot of overhead (code size, wrapper callbacks) and it's pretty aggressive with throwing
 * exceptions. The C++ api is a reasonable implementation, but Light Source Engine needs more control. If I knew
 * this at the beginning, I would have used the C api directly. The code is slowly being transitioned from using the
 * C++ napi to the C napi.
 */
namespace napix {

/**
 * Structure containing callback information, including args and function data pointer.
 *
 * @tparam N Maximum number of arguments supported.
 */
template<size_t N>
struct callback_info {
  napi_value thisContext{};
  void* data{};
  napi_value args[N]{};
  size_t arg_count{ N };

  /**
   * The number of arguments passed to the function.
   *
   * If the arguments passed exceeds N, the length will still be N.
   */
  int32_t length() const noexcept;

  /**
   * Access an argument by index.
   *
   * If the index is out of range, null is returned.
   */
  napi_value operator[](int32_t index) noexcept;

  /**
   * Cast the function data pointer to another object.
   *
   * @tparam T Type to cast data as
   */
  template<typename T>
  T* data_as() const noexcept;

  template<typename T>
  T* unwrap_this_as(napi_env env) const noexcept;
};

/**
 * Structure containing Buffer data.
 */
struct buffer_info {
  void* data{};
  size_t size{};

  template<typename T>
  T* as() const noexcept {
    return static_cast<T*>(this->data);
  }

  bool empty() const noexcept {
    return !this->data;
  }
};

struct name {
  const char* utf8;
  napi_value value;

  name(const char* utf8) noexcept: utf8(utf8), value(nullptr) {}
  name(const napi_value& value) noexcept: utf8(nullptr), value(value) {}
};

/**
 * Get callback info, including arguments and function data field, as a struct.
 */
template<size_t N>
callback_info<N> get_callback_info(napi_env env, napi_callback_info info) noexcept;

void* get_data_raw(napi_env env, napi_callback_info info) noexcept;

/**
 * Extract the data field from callback info and cast it to T. If data is not available, nullptr is
 * returned.
 */
template<typename T>
T* get_data(napi_env env, napi_callback_info info) noexcept;

napi_value get_this(napi_env env, napi_callback_info info) noexcept;

void* unwrap_this_as(napi_env env, napi_callback_info info) noexcept;

template<typename T>
T* unwrap_this_as(napi_env env, napi_callback_info info) noexcept;

template<typename T>
T* unwrap_as(napi_env env, napi_value wrapped) noexcept;

void throw_error(napi_env env, const char* message) noexcept;
bool has_pending_exception(napi_env env) noexcept;

/**
 * Creates a js value from a primitive. If the creation fails, nullptr is returned.
 */
napi_value to_value_or_null(napi_env env, int32_t value) noexcept;
napi_value to_value_or_null(napi_env env, uint32_t value) noexcept;
napi_value to_value_or_null(napi_env env, bool value) noexcept;
napi_value to_value_or_null(napi_env env, float value) noexcept;
napi_value to_value_or_null(napi_env env, const char* value) noexcept;

/**
 * Creates a js value from a primitive. If the creation fails, a js exception is thrown.
 */
napi_value to_value(napi_env env, int32_t value) noexcept;
napi_value to_value(napi_env env, uint32_t value) noexcept;
napi_value to_value(napi_env env, bool value) noexcept;
napi_value to_value(napi_env env, float value) noexcept;
napi_value to_value(napi_env env, const char* value) noexcept;
napi_value to_value(napi_env env, const std::string& value) noexcept;

/**
 * Conversion from napi_value to primitives
 */
int32_t as_int32(napi_env env, napi_value value, int32_t defaultValue) noexcept;
int32_t as_uint32(napi_env env, napi_value value, uint32_t defaultValue) noexcept;
bool as_bool(napi_env env, napi_value value, bool defaultValue) noexcept;
float as_float(napi_env env, napi_value value, float defaultValue) noexcept;
std::string as_string_utf8(napi_env env, napi_value str) noexcept;
napix::buffer_info as_buffer(napi_env env, napi_value value) noexcept;

std::string object_get(napi_env env, napi_value value, const char* prop) noexcept;

int32_t object_get_or(napi_env env, napi_value value, const char* prop, int32_t defaultValue) noexcept;
float object_get_or(napi_env env, napi_value value, const char* prop, float defaultValue) noexcept;
bool object_get_or(napi_env env, napi_value value, const char* prop, bool defaultValue) noexcept;

napi_value object_at(napi_env env, napi_value value, uint32_t index) noexcept;
int32_t object_at_or(napi_env env, napi_value value, uint32_t index, int32_t defaultValue) noexcept;
float object_at_or(napi_env env, napi_value value, uint32_t index, float defaultValue) noexcept;

napi_value object_new(napi_env env, const std::initializer_list<napi_property_descriptor>& props) noexcept;
napi_value array_new(napi_env env, size_t length = 0) noexcept;
template<class T, class Iterable>
napi_value array_from(napi_env env, Iterable iterable, napi_value(*toValue)(napi_env, T));

bool is_nullish(napi_env env, napi_value value) noexcept;
bool is_function(napi_env env, napi_value value) noexcept;
bool is_string(napi_env env, napi_value value) noexcept;
bool is_number(napi_env env, napi_value value) noexcept;
bool is_buffer(napi_env env, napi_value value) noexcept;

namespace js_class {

typedef void* (*create_native_callback)(napi_env env, napi_callback_info info);

napi_value define(
    napi_env env,
    const char* name,
    napi_callback constructor,
    const std::initializer_list<napi_property_descriptor>& props) noexcept;

napi_value define(
    napi_env env,
    const char* name,
    napi_callback constructor,
    size_t propCount,
    const napi_property_descriptor* props) noexcept;

napi_value constructor_helper(
    napi_env env,
    napi_callback_info info,
    create_native_callback create,
    napi_finalize finalizer) noexcept;

} // namespace js_class

namespace descriptor {

napi_property_descriptor instance_accessor(
    const name& name,
    napi_callback getter,
    napi_callback setter,
    napi_property_attributes attr = napi_writable) noexcept;
napi_property_descriptor instance_accessor(
    const name& name,
    napi_callback getter,
    napi_callback setter,
    intptr_t data,
    napi_property_attributes attr) noexcept;

napi_property_descriptor instance_method(const name& name, napi_callback method) noexcept;

napi_property_descriptor instance_value(
    napi_env env, const name& name, int32_t value, napi_property_attributes attr = napi_default) noexcept;
napi_property_descriptor instance_value(
    napi_env env, const name& name, const char* value, napi_property_attributes attr = napi_default) noexcept;
napi_property_descriptor instance_value(
    const name& name, napi_value value, napi_property_attributes attr = napi_default) noexcept;

} // namespace descriptor

} // namespace napix

#include "napix-inl.h"
