/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace lse {

enum AudioDestinationCapability {
    AudioDestinationCapabilityStop,
    AudioDestinationCapabilityResume,
    AudioDestinationCapabilityPause,
    AudioDestinationCapabilityVolume,
    AudioDestinationCapabilityFadeOut
};

class AudioDestinationInterface {
 public:
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetDecoders(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateAudioSource(const Napi::CallbackInfo& info) = 0;
    virtual void Resume(const Napi::CallbackInfo& info) = 0;
    virtual void Pause(const Napi::CallbackInfo& info) = 0;
    virtual void Stop(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetVolume(const Napi::CallbackInfo& info) = 0;
    virtual void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) = 0;
    virtual Napi::Value HasCapability(const Napi::CallbackInfo& info) = 0;

    virtual void Finalize() = 0;
};

using AudioDestinationInterfaceFactory = AudioDestinationInterface* (*)(const Napi::CallbackInfo& info);

class AudioDestination : public Napi::SafeObjectWrap<AudioDestination>, public AudioDestinationInterface {
 public:
    AudioDestination(const Napi::CallbackInfo& info);
    ~AudioDestination() override;

    void Constructor(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    Napi::Value GetDecoders(const Napi::CallbackInfo& info) override;
    Napi::Value CreateAudioSource(const Napi::CallbackInfo& info) override;
    void Resume(const Napi::CallbackInfo& info) override;
    void Pause(const Napi::CallbackInfo& info) override;
    void Stop(const Napi::CallbackInfo& info) override;
    Napi::Value GetVolume(const Napi::CallbackInfo& info) override;
    void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) override;
    Napi::Value HasCapability(const Napi::CallbackInfo& info) override;

    void Finalize() override;

    static Napi::Function GetClass(Napi::Env env);

    template<typename T>
    static Napi::Value Create(Napi::Env env, const std::initializer_list<napi_value>& args = {});

 private:
    AudioDestinationInterface* impl{};
};

template<typename T>
Napi::Value AudioDestination::Create(Napi::Env env, const std::initializer_list<napi_value>& args) {
    const AudioDestinationInterfaceFactory factory{
        [](const Napi::CallbackInfo& info) -> AudioDestinationInterface* {
          return new T(info);
        }
    };

    Napi::EscapableHandleScope scope(env);
    auto external{ Napi::External<void>::New(env, reinterpret_cast<void*>(factory)) };
    static std::vector<napi_value> constructorArgs;

    constructorArgs.clear();
    constructorArgs.push_back(external);

    for (auto arg : args) {
        constructorArgs.push_back(arg);
    }

    return scope.Escape(AudioDestination::GetClass(env).New(constructorArgs));
}

} // namespace lse
