/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "PlatformPlugin.h"

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::SafeObjectWrap;
using Napi::Value;

#define CHECK_IMPL(impl) if (!(impl)) { throw Napi::Error::New(info.Env(), "PlatformPlugin is not initialized!"); }

namespace lse {

PlatformPlugin::PlatformPlugin(const CallbackInfo& info) : Napi::SafeObjectWrap<PlatformPlugin>(info) {
}

PlatformPlugin::~PlatformPlugin() {
  if (this->impl) {
    this->impl->Finalize();
  }
}

void PlatformPlugin::Constructor(const CallbackInfo& info) {
  this->impl = Napi::ConstructorWithExternalFactory<PlatformPluginInterface, PlatformPluginInterfaceFactory>(
      info, "PlatformPlugin");
}

Value PlatformPlugin::GetKeyboard(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->GetKeyboard(info);
}

Value PlatformPlugin::GetGamepads(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->GetGamepads(info);
}

Value PlatformPlugin::GetCapabilities(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->GetCapabilities(info);
}

void PlatformPlugin::SetCallback(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->SetCallback(info);
}

void PlatformPlugin::ResetCallbacks(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->ResetCallbacks(info);
}

void PlatformPlugin::Attach(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->Attach(info);
}

void PlatformPlugin::Detach(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->Detach(info);
}

void PlatformPlugin::Destroy(const CallbackInfo& info) {
  if (this->impl) {
    this->impl->Destroy(info);
    this->impl->Finalize();
    this->impl = nullptr;
  }
}

Value PlatformPlugin::ProcessEvents(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->ProcessEvents(info);
}

Value PlatformPlugin::CreateGraphicsContext(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->CreateGraphicsContext(info);
}

Value PlatformPlugin::LoadGameControllerMappings(const CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->LoadGameControllerMappings(info);
}

Value PlatformPlugin::GetGameControllerMapping(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->GetGameControllerMapping(info);
}

void PlatformPlugin::Finalize() {
  throw std::runtime_error("Not implemented");
}

Function PlatformPlugin::GetClass(Napi::Env env) {
  static FunctionReference constructor;

  if (constructor.IsEmpty()) {
    HandleScope scope(env);

    constructor = DefineClass(env, "PlatformPlugin", true, {
        InstanceAccessor("capabilities", &PlatformPlugin::GetCapabilities, nullptr),
        InstanceMethod("getKeyboard", &PlatformPlugin::GetKeyboard),
        InstanceMethod("getGamepads", &PlatformPlugin::GetGamepads),
        InstanceMethod("processEvents", &PlatformPlugin::ProcessEvents),
        InstanceMethod("attach", &PlatformPlugin::Attach),
        InstanceMethod("detach", &PlatformPlugin::Detach),
        InstanceMethod("destroy", &PlatformPlugin::Destroy),
        InstanceMethod("setCallback", &PlatformPlugin::SetCallback),
        InstanceMethod("resetCallbacks", &PlatformPlugin::ResetCallbacks),
        InstanceMethod("createGraphicsContext", &PlatformPlugin::CreateGraphicsContext),
        InstanceMethod("loadGameControllerMappings", &PlatformPlugin::LoadGameControllerMappings),
        InstanceMethod("getGameControllerMapping", &PlatformPlugin::GetGameControllerMapping),
    });
  }

  return constructor.Value();
}

} // namespace lse

#undef CHECK_IMPL
