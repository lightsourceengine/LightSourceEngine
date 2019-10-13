/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>
#include <array>
#include <napi.h>
#include <ls/StageAdapter.h>

namespace ls {

class SDLKeyboard;
class SDLGamepad;

class SDLStageAdapter : public StageAdapter, public Napi::ObjectWrap<SDLStageAdapter> {
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

 public:
    explicit SDLStageAdapter(const Napi::CallbackInfo& info);
    virtual ~SDLStageAdapter() = default;

    static Napi::Function Constructor(Napi::Env env);

    Napi::Value GetKeyboard(const Napi::CallbackInfo& info) override;
    Napi::Value GetGamepads(const Napi::CallbackInfo& info) override;
    Napi::Value GetDisplays(const Napi::CallbackInfo& info) override;
    Napi::Value ProcessEvents(const Napi::CallbackInfo& info) override;
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    void SetCallback(const Napi::CallbackInfo& info) override;
    void ResetCallbacks(const Napi::CallbackInfo& info) override;
    Napi::Value AddGameControllerMappings(const Napi::CallbackInfo& info);

    std::unique_ptr<SceneAdapter> CreateSceneAdapter(const SceneAdapterConfig& config) override;
    Napi::Reference<Napi::Object>* AsReference() noexcept override { return this; };

 private:
    void Call(const StageCallback callbackId, const std::initializer_list<napi_value>& args);
    bool IsCallbackEmpty(const StageCallback callbackId);
    void SyncGamepads(Napi::Env env);
    void ClearGamepads();
    SDLGamepad* AddGamepad(Napi::Env env, int32_t index);
    Napi::Value GetGamepad(Napi::Env env, int32_t instanceId);
    void Attach(Napi::Env env);
    void Detach(Napi::Env env);
    bool DispatchQuit(Napi::Env env);
    void DispatchKeyboardKeyDown(Napi::Env env, int32_t scanCode, bool isRepeat);
    void DispatchKeyboardKeyUp(Napi::Env env, int32_t scanCode);
    void DispatchJoystickButtonUp(Napi::Env env, int32_t instanceId, int32_t buttonId);
    void DispatchJoystickButtonDown(Napi::Env env, int32_t instanceId, int32_t buttonId);
    void DispatchJotstickAxisMotion(Napi::Env env, int32_t instanceId, uint8_t axisIndex, float value);
    void DispatchJoystickHatMotion(Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue);
    void DispatchJoystickAdded(Napi::Env env, int32_t index);
    void DispatchJoystickRemoved(Napi::Env env, int32_t instanceId);

 private:
    static std::unordered_map<std::string, StageCallback> callbackMap;

    std::array<Napi::FunctionReference, StageCallbackCount> callbacks;
    SDLKeyboard* keyboard{};
    std::unordered_map<int32_t, SDLGamepad*> gamepadsByInstanceId{};
    bool isAttached{false};
};

} // namespace ls

#undef DeclareStageCallback
