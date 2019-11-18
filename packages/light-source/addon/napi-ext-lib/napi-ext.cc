/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "napi-ext.h"

namespace Napi {

Symbol SymbolFor(const Napi::Env& env, const std::string& key) {
    static FunctionReference symbolFor;

    if (symbolFor.IsEmpty()) {
        symbolFor.Reset(env.Global().Get("Symbol").As<Object>().Get("for").As<Function>(), 1);
        symbolFor.SuppressDestruct();
    }

    return symbolFor({ String::New(env, key) }).As<Symbol>();
}

std::string ObjectGetString(const Object& object, const std::string& key) {
    if (!object.Has(key)) {
        throw Error::New(object.Env(), "Expected property value to be Object: " + key);
    }

    auto value{ object.Get(key) };

    if (!value.IsString()) {
        throw Error::New(object.Env(), "Expected property value to be String: " + key);
    }

    return value.As<String>();
}

std::string ObjectGetStringOrEmpty(const Object& object, const std::string& key) {
    if (!object.Has(key)) {
        return "";
    }

    auto value{ object.Get(key) };

    if (!value.IsString()) {
        return "";
    }

    return value.As<Napi::String>();
}

std::string ToLowerCase(const Napi::String& text) {
    if (text.IsEmpty()) {
        return "";
    }

    auto env{ text.Env() };
    HandleScope scope(env);
    static FunctionReference toLowerCase;

    if (toLowerCase.IsEmpty()) {
        toLowerCase.Reset(env.Global()
                .Get("String").As<Function>()
                .Get("prototype").As<Object>()
                .Get("toLowerCase").As<Function>(),
            1);
        toLowerCase.SuppressDestruct();
    }

    return toLowerCase.Call(text, {}).As<String>();
}

std::string ToUpperCase(const Napi::String& text) {
    if (text.IsEmpty()) {
        return "";
    }

    auto env{ text.Env() };
    HandleScope scope(env);
    static FunctionReference toUpperCase;

    if (toUpperCase.IsEmpty()) {
        toUpperCase.Reset(env.Global()
                .Get("String").As<Function>()
                .Get("prototype").As<Object>()
                .Get("toUpperCase").As<Function>(),
            1);
        toUpperCase.SuppressDestruct();
    }

    return toUpperCase.Call(text, {}).As<String>();
}

void Call(const FunctionReference& func, const std::initializer_list<napi_value>& args) {
    if (!func.IsEmpty()) {
        func.Call(args);
    }
}

Value Call(const Napi::Env& env, const FunctionReference& func, const std::initializer_list<napi_value>& args) {
    return func.IsEmpty() ? env.Undefined() : func.Call(args);
}

Napi::Value RunScript(const Napi::Env& env, const String& script) {
    napi_value result{ nullptr };
    const auto status{ napi_run_script(env, script, &result) };

    NAPI_THROW_IF_FAILED(
        env,
        status,
        Value());

    return { env, result };
}

Napi::Value RunScript(const Napi::Env& env, const std::string& script) {
    return RunScript(env, String::New(env, script));
}

} // namespace Napi
