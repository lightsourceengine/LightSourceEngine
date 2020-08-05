/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefAudioPluginImpl.h"

#include <napi-ext.h>
#include <ls/AudioDestination.h>
#include <ls/AudioSource.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::HandleScope;
using Napi::NewStringArray;
using Napi::Number;
using Napi::Value;

namespace ls {

class RefAudioSourceImpl : public AudioSourceInterface {
 public:
    explicit RefAudioSourceImpl(const CallbackInfo& info) {}
    virtual ~RefAudioSourceImpl() = default;

    void Play(const CallbackInfo& info) override {}
    void Load(const CallbackInfo& info) override {}
    void Destroy(const CallbackInfo& info) override {}
    void SetVolume(const CallbackInfo& info, const Napi::Value& value) override {}
    Napi::Value HasCapability(const CallbackInfo& info) override { return Boolean::New(info.Env(), true); }
    Napi::Value GetVolume(const CallbackInfo& info) override { return Number::New(info.Env(), 0); }

    void Finalize() override { delete this; }
};

class RefAudioDestinationImpl : public AudioDestinationInterface {
 public:
    explicit RefAudioDestinationImpl(const CallbackInfo& info) {}
    virtual ~RefAudioDestinationImpl() = default;

    Napi::Value CreateAudioSource(const CallbackInfo& info) override {
        return AudioSource::Create<RefAudioSourceImpl>(info.Env());
    }

    Napi::Value HasCapability(const CallbackInfo& info) override { return Boolean::New(info.Env(), true); }
    Napi::Value GetVolume(const CallbackInfo& info) override { return Number::New(info.Env(), 0); }
    void Destroy(const CallbackInfo& info) override {}
    Value GetDecoders(const CallbackInfo& info) override { return NewStringArray(info.Env(), this->decoders); }
    void Resume(const CallbackInfo& info) override {}
    void Pause(const CallbackInfo& info) override {}
    void Stop(const CallbackInfo& info) override {}
    void SetVolume(const CallbackInfo& info, const Value& value) override {}

    void Finalize() override { delete this; }

 private:
    std::vector<std::string> decoders{ "WAVE" };
};

RefAudioPluginImpl::RefAudioPluginImpl(const Napi::CallbackInfo& info) {
    this->audioDevices = { "Reference" };
    this->Attach(info);
}

void RefAudioPluginImpl::Attach(const Napi::CallbackInfo& info) {
    this->isAttached = true;
}

void RefAudioPluginImpl::Detach(const Napi::CallbackInfo& info) {
    this->isAttached = false;
}

void RefAudioPluginImpl::Destroy(const Napi::CallbackInfo& info) {
}

Value RefAudioPluginImpl::IsAttached(const CallbackInfo& info) {
    return Boolean::New(info.Env(), this->isAttached);
}

Value RefAudioPluginImpl::GetAudioDevices(const CallbackInfo& info) {
    return NewStringArray(info.Env(), this->audioDevices);
}

Value RefAudioPluginImpl::CreateSampleAudioDestination(const CallbackInfo& info) {
    return AudioDestination::Create<RefAudioDestinationImpl>(info.Env());
}

Value RefAudioPluginImpl::CreateStreamAudioDestination(const CallbackInfo& info) {
    return AudioDestination::Create<RefAudioDestinationImpl>(info.Env());
}

void RefAudioPluginImpl::Finalize() {
    delete this;
}

} // namespace ls
