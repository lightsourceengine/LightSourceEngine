/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace lse {

class AudioPluginInterface {
 public:
    virtual void Attach(const Napi::CallbackInfo& info) = 0;
    virtual void Detach(const Napi::CallbackInfo& info) = 0;
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value IsAttached(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) = 0;

    virtual void Finalize() = 0;
};

using AudioPluginInterfaceFactory = AudioPluginInterface* (*)(const Napi::CallbackInfo& info);

class AudioPlugin : public Napi::SafeObjectWrap<AudioPlugin>, public AudioPluginInterface {
 public:
    AudioPlugin(const Napi::CallbackInfo& info);
    ~AudioPlugin() override;

 public:
    void Constructor(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    Napi::Value IsAttached(const Napi::CallbackInfo& info) override;
    Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) override;
    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
    Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

    void Finalize() override;

    static Napi::Function GetClass(Napi::Env env);

 private:
    AudioPluginInterface* impl{};
};

template<typename T>
Napi::Object AudioPluginInit(Napi::Env env, Napi::Object exports, const char* name) {
    auto createInstance{
        [](const Napi::CallbackInfo& info) -> Napi::Value {
            const AudioPluginInterfaceFactory factory{
                [](const Napi::CallbackInfo& info) -> AudioPluginInterface* {
                  return new T(info);
                }
            };

            Napi::EscapableHandleScope scope(info.Env());

            auto external{ Napi::External<void>::New(info.Env(), reinterpret_cast<void*>(factory)) };

            return scope.Escape(AudioPlugin::GetClass(info.Env()).New({ external }));
        }
    };

    exports.Set("name", Napi::String::New(env, name));
    exports.Set("type", Napi::String::New(env, "audio"));
    exports.Set("createInstance", Napi::Function::New(env, createInstance));

    return exports;
}

} // namespace lse
