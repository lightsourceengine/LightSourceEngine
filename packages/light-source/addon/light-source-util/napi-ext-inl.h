/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <algorithm>

namespace Napi {

template<typename T>
T ObjectGetNumberOrDefault(const Object& object, const char* key, T defaultValue) {
    if (!object.Has(key)) {
        return defaultValue;
    }

    auto value{ object.Get(key) };

    if (!value.IsNumber()) {
        return defaultValue;
    }

    return value.As<Number>();
}

template<typename Iterable>
Napi::Array StringArray(const Napi::Env& env, const Iterable& iterable) {
    auto result{ Array::New(env) };
    const auto appendString = [&env, &result](const std::string& value) {
        result[result.Length()] = String::New(env, value);
    };

    std::for_each(std::begin(iterable), std::end(iterable), appendString);

    return result;
}

} // namespace Napi
