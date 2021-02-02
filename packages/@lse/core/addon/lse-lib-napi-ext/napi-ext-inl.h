/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <algorithm>

namespace Napi {

template<typename T>
T ObjectGetNumberOrDefault(const Object& object, const std::string& key, T defaultValue) {
  if (!object.Has(key)) {
    return defaultValue;
  }

  auto value{ object.Get(key) };

  if (!value.IsNumber()) {
    return defaultValue;
  }

  return value.As<Number>();
}

template<typename T>
T ObjectGetNumberOrDefault(const Object& object, uint32_t index, T defaultValue) {
  if (!object.Has(index)) {
    return defaultValue;
  }

  auto value{ object.Get(index) };

  if (!value.IsNumber()) {
    return defaultValue;
  }

  return value.As<Number>();
}

template<typename T, typename F>
T* ConstructorWithExternalFactory(const Napi::CallbackInfo& info, const char* className) {
  auto env{ info.Env() };
  auto value{ info[0] };

  if (!value.IsExternal()) {
    throw Napi::Error::New(env, std::string(className) + ": expected an arg of type External");
  }

  auto factory{ value.As<Napi::External<void>>().Data() };

  if (!factory) {
    throw Napi::Error::New(env, std::string(className) + "External contains no factory method");
  }

  return reinterpret_cast<F>(factory)(info);
}

template<typename T>
T CastNumberOrDefault(const Napi::Value& value, T defaultValue) noexcept {
  if (value.IsNumber()) {
    try {
      return static_cast<T>(value.As<Napi::Number>());
    } catch (...) {
      // ignore and return default value
    }
  }

  return defaultValue;
}

template<typename ToType, typename SourceType>
Napi::Array ToArray(const Napi::Env& env, const std::vector<SourceType>& source) {
  uint32_t i{0};
  auto result{ Napi::Array::New(env, source.size()) };

  for (const auto& item : source) {
    result.Set(i++, ToType::New(env, item));
  }

  return result;
}

} // namespace Napi
