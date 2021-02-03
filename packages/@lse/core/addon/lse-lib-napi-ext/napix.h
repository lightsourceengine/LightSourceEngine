/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <node_api.h>
#include <string>
#include <memory>

#define NAPIX_TRY_STD(EXPR) \
  try { \
    (EXPR); \
  } catch (const std::exception& e) { \
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException(); \
    return {};\
  }

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
class napix final {
 public:
  /**
   * Get a arguments from callback info. If the argument is unavailable, it will be set to null.
   */
  static void get_args(napi_env env, napi_callback_info info, napi_value* arg) noexcept;

  /**
   * Extract the data field from callback info and cast it to T. If data is not available, nullptr is
   * returned.
   */
  template<typename T>
  static T* get_data(napi_env env, napi_callback_info info) noexcept {
    return static_cast<T*>(napix::get_data_raw(env, info));
  }

  /**
   * Creates a js value from a primitive. If the creation fails, nullptr is returned.
   */
  static napi_value to_value_or_null(napi_env env, int32_t value) noexcept;
  static napi_value to_value_or_null(napi_env env, bool value) noexcept;
  static napi_value to_value_or_null(napi_env env, float value) noexcept;
  static napi_value to_value_or_null(napi_env env, const char* value) noexcept;

  /**
   * Convert a js string to a native utf8 string.
   *
   * If str is not a js string type or an error occurs, empty string is returned.
   */
  static std::string as_string_utf8(napi_env env, napi_value str) noexcept;

  /**
   * Create a new js External object, a wrapper around a native pointer.
   */
  static napi_value new_external(napi_env env, void* data, napi_finalize finalizer) noexcept;

  /**
   * Create a new js External containing a pointer to a shared pointer.
   */
  template<typename T>
  static napi_value new_external_shared(napi_env env, std::shared_ptr<T>& ptr) noexcept {
    auto wrapper{ new std::shared_ptr<T>(ptr) };

    auto external = napix::new_external(env, wrapper,
        [](napi_env, void* data, void*) { delete static_cast<std::shared_ptr<T>*>(data); });

    if (!external) {
      delete wrapper;
    }

    return external;
  }

  /**
   * Get a share pointer from an External (created by new_external_shared).
   */
  template<typename T>
  static std::shared_ptr<T> get_external_shared(napi_env env, napi_value external) noexcept {
    void* result;

    if (!external || napi_get_value_external(env, external, &result) != napi_ok) {
      return {};
    }

    return *static_cast<std::shared_ptr<T>*>(result);
  }

 private:
  static void* get_data_raw(napi_env env, napi_callback_info info) noexcept;
};
