/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

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

} // namespace Napi
