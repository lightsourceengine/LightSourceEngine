/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <ls/Audio.h>

namespace ls {

enum AudioDestinationCapability {
    AudioDestinationCapabilityStop,
    AudioDestinationCapabilityResume,
    AudioDestinationCapabilityPause,
    AudioDestinationCapabilityVolume,
    AudioDestinationCapabilityFadeOut
};

class BaseAudioDestination {
 public:
    virtual ~BaseAudioDestination() = default;

    virtual Napi::Value GetDecoders(const Napi::CallbackInfo& info);
    virtual Napi::Value CreateAudioSource(const Napi::CallbackInfo& info) = 0;
    virtual void Resume(const Napi::CallbackInfo& info);
    virtual void Pause(const Napi::CallbackInfo& info);
    virtual void Stop(const Napi::CallbackInfo& info);
    virtual Napi::Value GetVolume(const Napi::CallbackInfo& info);
    virtual void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value);
    virtual Napi::Value HasCapability(const Napi::CallbackInfo& info);

 protected:
    template<typename T, typename ClassName>
    static Napi::Function ConstructorInternal(Napi::Env env);
    std::vector<std::string> decoders;
};

template<typename T, typename ClassName>
Napi::Function BaseAudioDestination::ConstructorInternal(Napi::Env env) {
    static Napi::FunctionReference constructor;

    if (constructor.IsEmpty()) {
        Napi::HandleScope scope(env);

        auto func = T::DefineClass(env, ClassName::Get(), {
            T::InstanceMethod("createAudioSource", &T::CreateAudioSource),
            T::InstanceMethod("resume", &T::Resume),
            T::InstanceMethod("pause", &T::Pause),
            T::InstanceMethod("stop", &T::Stop),
            T::InstanceMethod("hasCapability", &T::HasCapability),
            T::InstanceAccessor("volume", &T::GetVolume, &T::SetVolume),
            T::InstanceAccessor("decoders", &T::GetDecoders, nullptr),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

} // namespace ls