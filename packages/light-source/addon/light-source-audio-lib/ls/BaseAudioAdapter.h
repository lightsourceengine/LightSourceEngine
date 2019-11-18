/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/Audio.h>

namespace ls {

class BaseAudioAdapter : public AudioAdapter {
 public:
    virtual ~BaseAudioAdapter() = default;

    Napi::Value IsAttached(const Napi::CallbackInfo& info) override;
    Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) override;

    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
    Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

 protected:
    template<typename T>
    static Napi::Function GetClassInternal(Napi::Env env, const char* className);

 protected:
    bool isAttached{false};
    std::vector<std::string> audioDevices;
};

template<typename T>
Napi::Function BaseAudioAdapter::GetClassInternal(Napi::Env env, const char* className) {
    static Napi::FunctionReference constructor;

    if (constructor.IsEmpty()) {
        Napi::HandleScope scope(env);

        constructor = T::DefineClass(env, className, true, {
            T::InstanceMethod("attach", &T::Attach),
            T::InstanceMethod("detach", &T::Detach),
            T::InstanceMethod("createStreamAudioDestination", &T::CreateStreamAudioDestination),
            T::InstanceMethod("createSampleAudioDestination", &T::CreateSampleAudioDestination),
            T::InstanceAccessor("attached", &T::IsAttached),
            T::InstanceAccessor("devices", &T::GetAudioDevices),
        });
    }

    return constructor.Value();
}

} // namespace ls
