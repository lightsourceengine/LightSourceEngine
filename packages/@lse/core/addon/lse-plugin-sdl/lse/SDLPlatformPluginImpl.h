/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/CapabilitiesView.h>
#include <lse/PlatformPlugin.h>
#include <array>
#include <unordered_map>

namespace lse {

class SDLKeyboard;
class SDLGamepad;

class SDLPlatformPluginImpl final : public PlatformPluginInterface {
 public:
    explicit SDLPlatformPluginImpl(const Napi::CallbackInfo& info);
    virtual ~SDLPlatformPluginImpl();

 public: // javascript bindings
    Napi::Value GetKeyboard(const Napi::CallbackInfo& info) override;
    Napi::Value GetGamepads(const Napi::CallbackInfo& info) override;
    Napi::Value GetCapabilities(const Napi::CallbackInfo& info) override;
    void SetCallback(const Napi::CallbackInfo& info) override;
    void ResetCallbacks(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    Napi::Value LoadGameControllerMappings(const Napi::CallbackInfo& info) override;
    Napi::Value ProcessEvents(const Napi::CallbackInfo& info) override;
    Napi::Value CreateGraphicsContext(const Napi::CallbackInfo& info) override;

    void Finalize() override;

 private:
    enum StageCallback {
        StageCallbackGamepadConnected,
        StageCallbackGamepadDisconnected,
        StageCallbackKeyboardKeyUp,
        StageCallbackKeyboardKeyDown,
        StageCallbackGamepadButtonUp,
        StageCallbackGamepadButtonDown,
        StageCallbackHatUp,
        StageCallbackHatDown,
        StageCallbackGamepadAxisMotion,
        StageCallbackQuit,
        StageCallbackCount,
    };

    void Call(StageCallback callbackId, const std::initializer_list<napi_value>& args);
    bool IsCallbackEmpty(StageCallback callbackId);
    void SyncGamepads(Napi::Env env);
    void ClearGamepads();
    SDLGamepad* AddGamepad(Napi::Env env, int32_t index);
    Napi::Value GetGamepad(Napi::Env env, int32_t instanceId);
    bool DispatchQuit(Napi::Env env);
    void DispatchKeyboardKeyDown(Napi::Env env, int32_t scanCode, bool isRepeat);
    void DispatchKeyboardKeyUp(Napi::Env env, int32_t scanCode);
    void DispatchJoystickButtonUp(Napi::Env env, int32_t instanceId, int32_t buttonId);
    void DispatchJoystickButtonDown(Napi::Env env, int32_t instanceId, int32_t buttonId);
    void DispatchJoystickAxisMotion(Napi::Env env, int32_t instanceId, uint8_t axisIndex, float value);
    void DispatchJoystickHatMotion(Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue);
    void DispatchJoystickAdded(Napi::Env env, int32_t index);
    void DispatchJoystickRemoved(Napi::Env env, int32_t instanceId);
    void Init(Napi::Env env);
    Capabilities DetermineCapabilities(Napi::Env env);

 private:
    static std::unordered_map<std::string, StageCallback> callbackMap;

    Capabilities capabilities;
    Napi::ObjectReference capabilitiesRef;
    std::array<Napi::FunctionReference, StageCallbackCount> callbacks;
    SDLKeyboard* keyboard{};
    std::unordered_map<int32_t, SDLGamepad*> gamepadsByInstanceId{};
    bool isAttached{false};
};

} // namespace lse

#undef DeclareStageCallback
