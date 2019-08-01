/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <unordered_set>
#include <fmt/format.h>

namespace ls {

Napi::Symbol SymbolFor(Napi::Env env, const char* key);
std::string GetString(const Napi::Object options, const char* name);
std::string GetStringOrEmpty(const Napi::Object options, const char* name);

template<typename T>
T GetNumberOrDefault(const Napi::Object options, const char* name, T defaultValue);

template<typename T>
class AsyncWork {
 public:
    typedef std::function<std::shared_ptr<T> (Napi::Env)> ExecuteFunction;
    typedef std::function<void(Napi::Env, std::shared_ptr<T>, napi_status, const std::string&)> CompleteFunction;

 public:
    AsyncWork(Napi::Env env, const std::string& resourceName, ExecuteFunction execute, CompleteFunction complete);
    ~AsyncWork();

 private:
    static void OnExecute(napi_env env, void* self);
    static void OnComplete(napi_env env, napi_status status, void* self);

 private:
    static std::unordered_set<AsyncWork<T>*> activeWork;

    Napi::Env env;
    ExecuteFunction execute;
    CompleteFunction complete;
    napi_async_work work{};
    napi_status status{napi_ok};
    std::string message;
    std::shared_ptr<T> result;
};

} // namespace ls

#include "napi-ext-inl.h"
