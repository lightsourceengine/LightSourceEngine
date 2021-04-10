/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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

template<class T, class Iterable>
napi_value array_from(napi_env env, Iterable iterable, napi_value(*toValue)(napi_env, T)) {
  napi_value target{array_new(env, iterable.size())};
  uint32_t index{0};

  if (!target) {
    return {};
  }

  for (auto& i : iterable) {
    napi_set_element(env, target, index++, toValue(env, i));
  }

  return target;
}

} // namespace napix
