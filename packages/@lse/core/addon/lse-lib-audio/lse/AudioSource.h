/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace lse {

enum AudioSourceCapability {
    AudioSourceCapabilityVolume,
    AudioSourceCapabilityLoop,
    AudioSourceCapabilityFadeIn
};

class AudioSourceInterface {
 public:
    virtual void Load(const Napi::CallbackInfo& info) = 0;
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual void Play(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetVolume(const Napi::CallbackInfo& info) = 0;
    virtual void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) = 0;
    virtual Napi::Value HasCapability(const Napi::CallbackInfo& info) = 0;

    virtual void Finalize() = 0;
};

using AudioSourceInterfaceFactory = AudioSourceInterface* (*)(const Napi::CallbackInfo& info);

class AudioSource : public Napi::SafeObjectWrap<AudioSource>, public AudioSourceInterface {
 public:
    AudioSource(const Napi::CallbackInfo& info);
    ~AudioSource() override;

    void Constructor(const Napi::CallbackInfo& info) override;
    void Load(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    void Play(const Napi::CallbackInfo& info) override;
    Napi::Value GetVolume(const Napi::CallbackInfo& info) override;
    void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) override;
    Napi::Value HasCapability(const Napi::CallbackInfo& info) override;

    void Finalize() override;

    static Napi::Function GetClass(Napi::Env env);

    template<typename T>
    static Napi::Value Create(Napi::Env env, const std::initializer_list<napi_value>& args = {});

 private:
    AudioSourceInterface* impl{};
};

template<typename T>
Napi::Value AudioSource::Create(Napi::Env env, const std::initializer_list<napi_value>& args) {
    const AudioSourceInterfaceFactory factory{
        [](const Napi::CallbackInfo& info) -> AudioSourceInterface* {
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

    return scope.Escape(AudioSource::GetClass(env).New(constructorArgs));
}

} // namespace lse
