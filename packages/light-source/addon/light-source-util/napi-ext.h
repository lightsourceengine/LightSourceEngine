/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <fmt/format.h>

namespace ls {

inline
Napi::Symbol SymbolFor(Napi::Env env, const char* key) {
    static Napi::FunctionReference symbolFor;

    if (symbolFor.IsEmpty()) {
        symbolFor.Reset(env.Global().Get("Symbol").As<Napi::Object>().Get("for").As<Napi::Function>(), 1);
        symbolFor.SuppressDestruct();
    }

    return symbolFor({ Napi::String::New(env, key) }).As<Napi::Symbol>();
}

inline
std::string GetString(Napi::Object options, const char* name) {
    if (!options.Has(name)) {
        throw Napi::Error::New(options.Env(), fmt::format("Expected '{}' property in Object.", name));
    }

    auto value{ options.Get(name) };

    if (!value.IsString()) {
        throw Napi::Error::New(options.Env(), fmt::format("Expected '{}' property in Object to be a String.", name));
    }

    return value.As<Napi::String>();
}

template<typename T>
T GetNumberOrDefault(Napi::Object options, const char* name, T defaultValue) {
    if (!options.Has(name)) {
        return defaultValue;
    }

    auto value{ options.Get(name) };

    if (!value.IsNumber()) {
        return defaultValue;
    }

    return value.As<Napi::Number>();
}

typedef std::function<void(Napi::Env env)> ExecuteFunction;
typedef std::function<void(Napi::Env env, napi_status status, const std::string& message)> CompleteFunction;

class AsyncWork {
 public:
    AsyncWork(Napi::Env env, const std::string& resourceName, ExecuteFunction execute, CompleteFunction complete);
    ~AsyncWork();

    void Cancel();

 private:
    static void OnExecute(napi_env env, void* self);
    static void OnComplete(napi_env env, napi_status status, void* self);

 private:
    ExecuteFunction execute;
    CompleteFunction complete;
    napi_async_work work{};
    napi_status result{napi_ok};
    std::string errorMessage;
    Napi::Env env;
};

} // namespace ls
