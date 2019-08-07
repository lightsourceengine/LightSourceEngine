/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "napi-ext.h"
#include <fmt/format.h>

namespace Napi {

Symbol SymbolFor(Napi::Env env, const char* key) {
    static FunctionReference symbolFor;

    if (symbolFor.IsEmpty()) {
        symbolFor.Reset(env.Global().Get("Symbol").As<Object>().Get("for").As<Function>(), 1);
        symbolFor.SuppressDestruct();
    }

    return symbolFor({ String::New(env, key) }).As<Symbol>();
}

std::string ObjectGetString(const Object& object, const char* key) {
    if (!object.Has(key)) {
        throw Error::New(object.Env(), fmt::format("Expected '{}' property in Object.", key));
    }

    auto value{ object.Get(key) };

    if (!value.IsString()) {
        throw Error::New(object.Env(), fmt::format("Expected '{}' property in Object to be a String.", key));
    }

    return value.As<String>();
}

std::string ObjectGetStringOrEmpty(const Object& object, const char* key) {
    if (!object.Has(key)) {
        return "";
    }

    auto value{ object.Get(key) };

    if (!value.IsString()) {
        return "";
    }

    return value.As<Napi::String>();
}

} // namespace Napi
