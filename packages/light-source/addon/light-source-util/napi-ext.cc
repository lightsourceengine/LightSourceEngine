/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "napi-ext.h"

using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::Object;
using Napi::String;
using Napi::Symbol;

namespace ls {

Symbol SymbolFor(Napi::Env env, const char* key) {
    static FunctionReference symbolFor;

    if (symbolFor.IsEmpty()) {
        symbolFor.Reset(env.Global().Get("Symbol").As<Object>().Get("for").As<Function>(), 1);
        symbolFor.SuppressDestruct();
    }

    return symbolFor({ String::New(env, key) }).As<Symbol>();
}

std::string GetString(const Object options, const char* name) {
    if (!options.Has(name)) {
        throw Error::New(options.Env(), fmt::format("Expected '{}' property in Object.", name));
    }

    auto value{ options.Get(name) };

    if (!value.IsString()) {
        throw Error::New(options.Env(), fmt::format("Expected '{}' property in Object to be a String.", name));
    }

    return value.As<String>();
}

std::string GetStringOrEmpty(const Object options, const char* name) {
    if (!options.Has(name)) {
        return "";
    }

    auto value{ options.Get(name) };

    if (!value.IsString()) {
        return "";
    }

    return value.As<Napi::String>();
}

} // namespace ls
