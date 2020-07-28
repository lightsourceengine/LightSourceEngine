/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefStageAdapter.h"
#include "RefSceneAdapter.h"
#include <ls/CapabilitiesView.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::Persistent;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

RefStageAdapter::RefStageAdapter(const CallbackInfo& info) : SafeObjectWrap<RefStageAdapter>(info) {
    auto env{ info.Env() };
    HandleScope scope(env);
    Capabilities capabilities{};
    DisplayMode displayMode{ 1280, 720 };

    capabilities.displays = {
        {"mock display", displayMode, { displayMode } }
    };

    this->capabilitiesRef = Persistent(ToCapabilitiesView(env, capabilities));
}

Function RefStageAdapter::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "RefStageAdapter", true, {
            StaticValue("type", String::New(env, "graphics"), napi_default),
            StaticValue("SceneAdapter", RefSceneAdapter::GetClass(env), napi_default),
            InstanceMethod("getKeyboard", &RefStageAdapter::GetKeyboard),
            InstanceMethod("getGamepads", &RefStageAdapter::GetGamepads),
            InstanceAccessor("capabilities", &RefStageAdapter::GetCapabilities, nullptr),
            InstanceMethod("processEvents", &RefStageAdapter::ProcessEvents),
            InstanceMethod("attach", &RefStageAdapter::Attach),
            InstanceMethod("detach", &RefStageAdapter::Detach),
            InstanceMethod("destroy", &RefStageAdapter::Destroy),
            InstanceMethod("setCallback", &RefStageAdapter::SetCallback),
            InstanceMethod("resetCallbacks", &RefStageAdapter::ResetCallbacks),
        });
    }

    return constructor.Value();
}

Value RefStageAdapter::GetKeyboard(const CallbackInfo& info) {
    return info.Env().Null();
}

Value RefStageAdapter::GetGamepads(const CallbackInfo& info) {
    return Array::New(info.Env());
}

void RefStageAdapter::Attach(const CallbackInfo& info) {
}

void RefStageAdapter::Detach(const CallbackInfo& info) {
}

void RefStageAdapter::Destroy(const CallbackInfo& info) {
}

void RefStageAdapter::SetCallback(const CallbackInfo& info) {
}

void RefStageAdapter::ResetCallbacks(const CallbackInfo& info) {
}


Napi::Value RefStageAdapter::GetCapabilities(const Napi::CallbackInfo& info) {
    return this->capabilitiesRef.Value();
}

Value RefStageAdapter::ProcessEvents(const CallbackInfo& info) {
    return Boolean::New(info.Env(), true);
}

} // namespace ls
