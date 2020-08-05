/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace ls {

class PlatformPluginInterface {
 public:
    virtual Napi::Value GetKeyboard(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetGamepads(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value GetCapabilities(const Napi::CallbackInfo& info) = 0;
    virtual void SetCallback(const Napi::CallbackInfo& info) = 0;
    virtual void ResetCallbacks(const Napi::CallbackInfo& info) = 0;
    virtual void Attach(const Napi::CallbackInfo& info) = 0;
    virtual void Detach(const Napi::CallbackInfo& info) = 0;
    virtual void Destroy(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value ProcessEvents(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateGraphicsContext(const Napi::CallbackInfo& info) = 0;
    virtual void AddGameControllerMappings(const Napi::CallbackInfo& info) = 0;

    virtual void Finalize() = 0;
};

using PlatformPluginInterfaceFactory = PlatformPluginInterface* (*)(const Napi::CallbackInfo& info);

class PlatformPlugin : public Napi::SafeObjectWrap<PlatformPlugin>, PlatformPluginInterface {
 public:
    PlatformPlugin(const Napi::CallbackInfo& info);
    ~PlatformPlugin() override;

    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value GetKeyboard(const Napi::CallbackInfo& info) override;
    Napi::Value GetGamepads(const Napi::CallbackInfo& info) override;
    Napi::Value GetCapabilities(const Napi::CallbackInfo& info) override;
    void SetCallback(const Napi::CallbackInfo& info) override;
    void ResetCallbacks(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    Napi::Value ProcessEvents(const Napi::CallbackInfo& info) override;
    Napi::Value CreateGraphicsContext(const Napi::CallbackInfo& info) override;
    void AddGameControllerMappings(const Napi::CallbackInfo& info) override;

    void Finalize() override;

    static Napi::Function GetClass(Napi::Env env);

 private:
  PlatformPluginInterface* impl{};
};

template<typename T>
Napi::Object PlatformPluginInit(Napi::Env env, Napi::Object exports, const char* name) {
    auto createInstance{
        [](const Napi::CallbackInfo& info) -> Napi::Value {
            const PlatformPluginInterfaceFactory factory{
                [](const Napi::CallbackInfo& info) -> PlatformPluginInterface* {
                    return new T(info);
                }
            };

            Napi::EscapableHandleScope scope(info.Env());
            auto external{ Napi::External<void>::New(info.Env(), reinterpret_cast<void*>(factory)) };

            return scope.Escape(PlatformPlugin::GetClass(info.Env()).New({ external }));
        }
    };

    exports.Set("name", Napi::String::New(env, name));
    exports.Set("type", Napi::String::New(env, "platform"));
    exports.Set("createInstance", Napi::Function::New(env, createInstance));

    return exports;
}

} // namespace ls
