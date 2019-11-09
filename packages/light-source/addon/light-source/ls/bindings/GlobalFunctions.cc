/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "GlobalFunctions.h"
#include <ls/StyleValue.h>
#include <ls/SceneNode.h>
#include <algorithm>
#include <cctype>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::Number;
using Napi::Object;
using Napi::String;

namespace ls {
namespace bindings {

Napi::Value ParseColor(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };

    if (!info[0].IsString()) {
        throw Error::New(env, "parseColor(): String argument expected");
    }

    std::string str{ info[0].As<String>() };

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    const auto color{ StyleValueColor::Parse(str) };

    if (color.empty()) {
        return env.Undefined();
    } else {
        return Number::New(env, color.value);
    }
}

void InitGlobalFunctions(Napi::Env env, Napi::Object exports) {
    auto addFunction = [&](const char* name, auto func) {
        exports[name] = Function::New(env, func, name);
    };

    addFunction("getSceneNodeInstanceCount", &ls::SceneNode::GetInstanceCount);
    addFunction("parseColor", &ParseColor);
}

} // namespace bindings
} // namespace ls
