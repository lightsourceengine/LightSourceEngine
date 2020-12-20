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

template<typename Iterable>
Napi::Array NewStringArray(const Napi::Env& env, const Iterable& iterable) {
    auto result{ Array::New(env) };
    const auto appendString = [&env, &result](const std::string& value) {
        result[result.Length()] = String::New(env, value);
    };

    std::for_each(std::begin(iterable), std::end(iterable), appendString);

    return result;
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

} // namespace Napi
