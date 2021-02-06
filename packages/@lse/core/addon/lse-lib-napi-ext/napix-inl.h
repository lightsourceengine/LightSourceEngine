/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

namespace napix {

template<size_t N>
template<typename T>
T* callback_info<N>::unwrap_this_as(napi_env env) const noexcept {
  return unwrap_as<T>(env, this->thisContext);
}

template<size_t N>
template<typename T>
T* callback_info<N>::data_as() const noexcept {
  return static_cast<T*>(this->data);
}

template<size_t N>
napi_value callback_info<N>::operator[](int32_t index) noexcept {
  if (index >= 0 && index < static_cast<int32_t>(this->arg_count)) {
    return this->args[index];
  }
  return {};
}

template<size_t N>
int32_t callback_info<N>::length() const noexcept {
  return this->arg_count;
}

template<typename T>
T* unwrap_as(napi_env env, napi_value wrapped) noexcept {
  void* native{};

  if (napi_unwrap(env, wrapped, &native) != napi_ok || !wrapped) {
    return {};
  }

  return static_cast<T*>(native);
}

template<typename T>
T* unwrap_this_as(napi_env env, napi_callback_info info) noexcept {
  return static_cast<T*>(unwrap_this_as(env, info));
}

template<typename T>
T* get_data(napi_env env, napi_callback_info info) noexcept {
  return static_cast<T*>(get_data_raw(env, info));
}

template<size_t N>
callback_info<N> get_callback_info(napi_env env, napi_callback_info info) noexcept {
  napix::callback_info<N> callback_info;

  napi_get_cb_info(
      env,
      info,
      &callback_info.arg_count,
      &callback_info.args[0],
      &callback_info.thisContext,
      &callback_info.data);

  return callback_info;
}

} // namespace napix
