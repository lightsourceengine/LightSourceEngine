/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/Audio.h>

namespace ls {

enum AudioSourceCapability {
    AudioSourceCapabilityVolume,
    AudioSourceCapabilityLoop,
    AudioSourceCapabilityFadeIn
};

class BaseAudioSource : public virtual Napi::SafeObjectWrapBase {
 public:
    virtual void Load(const Napi::CallbackInfo& info) = 0;
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual void Play(const Napi::CallbackInfo& info);
    virtual Napi::Value GetVolume(const Napi::CallbackInfo& info);
    virtual void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value);
    virtual Napi::Value HasCapability(const Napi::CallbackInfo& info);

 protected:
    template<typename T>
    static Napi::Function GetClassInternal(Napi::Env env, const char* className);
};

template<typename T>
Napi::Function BaseAudioSource::GetClassInternal(Napi::Env env, const char* className) {
    static Napi::FunctionReference constructor;

    if (constructor.IsEmpty()) {
        Napi::HandleScope scope(env);

        constructor = T::DefineClass(env, className, true, {
            T::InstanceMethod("load", &T::Load),
            T::InstanceMethod("destroy", &T::Destroy),
            T::InstanceMethod("play", &T::Play),
            T::InstanceMethod("hasCapability", &T::HasCapability),
            T::InstanceAccessor("volume", &T::GetVolume, &T::SetVolume),
        });
    }

    return constructor.Value();
}

} // namespace ls
