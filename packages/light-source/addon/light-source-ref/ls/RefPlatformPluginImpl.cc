/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefPlatformPluginImpl.h"

#include <ls/CapabilitiesView.h>

#include "RefGraphicsContextImpl.h"

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::EscapableHandleScope;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::Persistent;
using Napi::String;
using Napi::Value;

namespace ls {

RefPlatformPluginImpl::RefPlatformPluginImpl(const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);
    Capabilities capabilities{};
    DisplayMode displayMode{ 1280, 720 };

    capabilities.displays = {
        {"mock display", displayMode, { displayMode } }
    };

    this->capabilitiesRef = Persistent(ToCapabilitiesView(env, capabilities));
}

Value RefPlatformPluginImpl::GetKeyboard(const CallbackInfo& info) {
    return info.Env().Null();
}

Value RefPlatformPluginImpl::GetGamepads(const CallbackInfo& info) {
    return Array::New(info.Env());
}

void RefPlatformPluginImpl::Attach(const CallbackInfo& info) {
}

void RefPlatformPluginImpl::Detach(const CallbackInfo& info) {
}

void RefPlatformPluginImpl::Destroy(const CallbackInfo& info) {
}

void RefPlatformPluginImpl::SetCallback(const CallbackInfo& info) {
}

void RefPlatformPluginImpl::ResetCallbacks(const CallbackInfo& info) {
}

Napi::Value RefPlatformPluginImpl::GetCapabilities(const Napi::CallbackInfo& info) {
    return this->capabilitiesRef.Value();
}

Value RefPlatformPluginImpl::ProcessEvents(const CallbackInfo& info) {
    return Boolean::New(info.Env(), true);
}

Value RefPlatformPluginImpl::CreateGraphicsContext(const CallbackInfo& info) {
    return GraphicsContext::Create<RefGraphicsContextImpl>(info.Env(), info[0]);
}

void RefPlatformPluginImpl::AddGameControllerMappings(const CallbackInfo& info) {
}

void RefPlatformPluginImpl::Finalize() {
    delete this;
}

} // namespace ls
