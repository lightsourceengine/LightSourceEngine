/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <ls/Audio.h>

namespace ls {

enum AudioSourceCapability {
    AudioSourceCapabilityVolume,
    AudioSourceCapabilityLoop,
    AudioSourceCapabilityFadeIn
};

class BaseAudioSource {
 public:
    virtual ~BaseAudioSource() = default;

    virtual void Load(const Napi::CallbackInfo& info) = 0;
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual void Play(const Napi::CallbackInfo& info);
    virtual Napi::Value GetVolume(const Napi::CallbackInfo& info);
    virtual void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value);
    virtual Napi::Value HasCapability(const Napi::CallbackInfo& info);

 protected:
    template<typename T, typename ClassName>
    static Napi::Function ConstructorInternal(Napi::Env env);
};

template<typename T, typename ClassName>
Napi::Function BaseAudioSource::ConstructorInternal(Napi::Env env) {
    static Napi::FunctionReference constructor;

    if (constructor.IsEmpty()) {
        Napi::HandleScope scope(env);

        auto func = T::DefineClass(env, ClassName::Get(), {
            T::InstanceMethod("load", &T::Load),
            T::InstanceMethod("destroy", &T::Destroy),
            T::InstanceMethod("play", &T::Play),
            T::InstanceMethod("hasCapability", &T::HasCapability),
            T::InstanceAccessor("volume", &T::GetVolume, &T::SetVolume),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

} // namespace ls
