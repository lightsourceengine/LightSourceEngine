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
   * Structure containing callback information, including args and function data pointer.
   *
   * @tparam N Maximum number of arguments supported.
   */
  template<size_t N>
  struct callback_info {
    std::array<napi_value, N> args{};
    void* data{};
    size_t arg_count{N};

    /**
     * The number of arguments passed to the function.
     *
     * If the arguments passed exceeds N, the length will still be N.
     */
    int32_t length() const noexcept {
      return this->args.size();
    }

    /**
     * Access an argument by index.
     *
     * If the index is out of range, null is returned.
     */
    napi_value operator[](int32_t index) noexcept {
      if (index >= 0 && index < static_cast<int32_t>(this->args.size())) {
        return this->args[index];
      }
      return {};
    }

    /**
     * Cast the function data pointer to another object.
     *
     * @tparam T Type to cast data as
     */
    template<typename T>
    T* data_as() const noexcept {
      return static_cast<T*>(this->data);
    }
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

 public:
  /**
   * Get callback info, including arguments and function data field, as a struct.
   */
  template<size_t N>
  static callback_info<N> get_callback_info(napi_env env, napi_callback_info info) noexcept {
    napix::callback_info<N> callback_info;

    napi_get_cb_info(env, info, &callback_info.arg_count, &callback_info.args[0], nullptr, &callback_info.data);

    return callback_info;
  }

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
   * Creates a js value from a primitive. If the creation fails, a js exception is thrown.
   */
  static napi_value to_value(napi_env env, int32_t value) noexcept;
  static napi_value to_value(napi_env env, bool value) noexcept;
  static napi_value to_value(napi_env env, float value) noexcept;
  static napi_value to_value(napi_env env, const char* value) noexcept;

  static napix::buffer_info as_buffer(napi_env env, napi_value value) noexcept;

  static int32_t as_int32(napi_env env, napi_value value, int32_t defaultValue) noexcept;

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
