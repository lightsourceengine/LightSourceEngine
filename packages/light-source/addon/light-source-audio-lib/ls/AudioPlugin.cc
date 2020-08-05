/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "AudioPlugin.h"

#define CHECK_IMPL(impl) if (!(impl)) { throw Napi::Error::New(info.Env(), "AudioPlugin is not initialized!"); }

namespace ls {

Napi::Function AudioPlugin::GetClass(Napi::Env env) {
    static Napi::FunctionReference constructor;

    if (constructor.IsEmpty()) {
        Napi::HandleScope scope(env);

        constructor = AudioPlugin::DefineClass(env, "AudioPlugin", true, {
            AudioPlugin::InstanceMethod("attach", &AudioPlugin::Attach),
            AudioPlugin::InstanceMethod("detach", &AudioPlugin::Detach),
            AudioPlugin::InstanceMethod("destroy", &AudioPlugin::Destroy),
            AudioPlugin::InstanceMethod("createStreamAudioDestination", &AudioPlugin::CreateStreamAudioDestination),
            AudioPlugin::InstanceMethod("createSampleAudioDestination", &AudioPlugin::CreateSampleAudioDestination),
            AudioPlugin::InstanceAccessor("attached", &AudioPlugin::IsAttached),
            AudioPlugin::InstanceAccessor("devices", &AudioPlugin::GetAudioDevices),
        });
    }

    return constructor.Value();
}

AudioPlugin::AudioPlugin(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<AudioPlugin>(info) {
}

AudioPlugin::~AudioPlugin() {
    if (this->impl) {
        this->impl->Finalize();
    }
}

void AudioPlugin::Constructor(const Napi::CallbackInfo& info) {
    this->impl = Napi::ConstructorWithExternalFactory<AudioPluginInterface, AudioPluginInterfaceFactory>(
        info, "AudioPlugin");
}

void AudioPlugin::Finalize() {
    throw std::runtime_error("Not implemented");
}

void AudioPlugin::Attach(const Napi::CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    this->impl->Attach(info);
}

void AudioPlugin::Detach(const Napi::CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    this->impl->Detach(info);
}

void AudioPlugin::Destroy(const Napi::CallbackInfo& info) {
    if (this->impl) {
        this->impl->Destroy(info);
        this->impl->Finalize();
        this->impl = nullptr;
    }
}

Napi::Value AudioPlugin::IsAttached(const Napi::CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->IsAttached(info);
}

Napi::Value AudioPlugin::GetAudioDevices(const Napi::CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->GetAudioDevices(info);
}

Napi::Value AudioPlugin::CreateSampleAudioDestination(const Napi::CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->CreateSampleAudioDestination(info);
}

Napi::Value AudioPlugin::CreateStreamAudioDestination(const Napi::CallbackInfo& info) {
    CHECK_IMPL(this->impl);
    return this->impl->CreateStreamAudioDestination(info);
}

} // namespace ls

#undef CHECK_IMPL
