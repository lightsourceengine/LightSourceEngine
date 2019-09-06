/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <Audio.h>

namespace ls {

class BaseAudioAdapter : public AudioAdapter {
 public:
    virtual ~BaseAudioAdapter() = default;

    Napi::Value IsAttached(const Napi::CallbackInfo& info) override;
    Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) override;

    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
    Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

 protected:
    template<typename T, typename ClassName>
    static Napi::Function ConstructorInternal(Napi::Env env);

 protected:
    bool isAttached{false};
    std::vector<std::string> audioDevices;
};

template<typename T, typename ClassName>
Napi::Function BaseAudioAdapter::ConstructorInternal(Napi::Env env) {
    static Napi::FunctionReference constructor;

    if (constructor.IsEmpty()) {
        Napi::HandleScope scope(env);

        auto func = T::DefineClass(env, ClassName::Get(), {
            T::InstanceMethod("attach", &T::Attach),
            T::InstanceMethod("detach", &T::Detach),
            T::InstanceMethod("createStreamAudioDestination", &T::CreateStreamAudioDestination),
            T::InstanceMethod("createSampleAudioDestination", &T::CreateSampleAudioDestination),
            T::InstanceAccessor("attached", &T::IsAttached, nullptr),
            T::InstanceAccessor("devices", &T::GetAudioDevices, nullptr),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

} // namespace ls
