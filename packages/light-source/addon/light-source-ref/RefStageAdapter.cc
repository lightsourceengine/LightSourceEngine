/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefStageAdapter.h"
#include "RefSceneAdapter.h"
#include <fmt/format.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

RefStageAdapter::RefStageAdapter(const CallbackInfo& info) : ObjectWrap<RefStageAdapter>(info) {
}

Function RefStageAdapter::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "RefStageAdapter", {
            InstanceMethod("getKeyboard", &RefStageAdapter::GetKeyboard),
            InstanceMethod("getGamepads", &RefStageAdapter::GetGamepads),
            InstanceMethod("getDisplays", &RefStageAdapter::GetDisplays),
            InstanceMethod("resetCallbacks", &RefStageAdapter::ResetCallbacks),
            InstanceMethod("processEvents", &RefStageAdapter::ProcessEvents),
            InstanceMethod("attach", &RefStageAdapter::Attach),
            InstanceMethod("detach", &RefStageAdapter::Detach),
            InstanceMethod("destroy", &RefStageAdapter::Destroy),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value RefStageAdapter::GetKeyboard(const CallbackInfo& info) {
    return info.Env().Null();
}

Value RefStageAdapter::GetGamepads(const CallbackInfo& info) {
    return Array::New(info.Env());
}

Value RefStageAdapter::GetDisplays(const CallbackInfo& info) {
    return Array::New(info.Env());
}

void RefStageAdapter::Attach(const CallbackInfo& info) {
}

void RefStageAdapter::Detach(const CallbackInfo& info) {
}

void RefStageAdapter::Destroy(const CallbackInfo& info) {
}

void RefStageAdapter::ResetCallbacks(const CallbackInfo& info) {
}

std::unique_ptr<SceneAdapter> RefStageAdapter::CreateSceneAdapter(const SceneAdapterConfig& config) {
    return std::make_unique<RefSceneAdapter>(config);
}

Value RefStageAdapter::ProcessEvents(const CallbackInfo& info) {
    return Boolean::New(info.Env(), true);
}

} // namespace ls
