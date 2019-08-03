/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>
#include <napi.h>
#include <StageAdapter.h>

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

    std::unique_ptr<SceneAdapter> CreateSceneAdapter(const SceneAdapterConfig& config) override;

 private:
    inline void Call(const StageCallback callbackId, const std::initializer_list<napi_value>& args);
    inline bool IsCallbackEmpty(const StageCallback callbackId);
    void SyncGamepads(Napi::Env env);
    void ClearGamepads();
    SDLGamepad* AddGamepad(Napi::Env env, int32_t index);
    inline Napi::Value GetGamepad(Napi::Env env, int32_t instanceId);
    void HandleJoystickHatMotion(Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue);
    void HandleJoystickAdded(Napi::Env env, int32_t index);
    void HandleJoystickRemoved(Napi::Env env, int32_t instanceId);
    void Attach(Napi::Env env);
    void Detach(Napi::Env env);

 private:
    static constexpr int NUM_EVENTS_PER_FRAME{20};
    static std::unordered_map<std::string, StageCallback> callbackMap;

    Napi::FunctionReference callbacks[StageCallbackCount];
    SDLKeyboard* keyboard{};
    std::unordered_map<int32_t, SDLGamepad*> gamepadsByInstanceId{};
    bool isAttached{false};
};

} // namespace ls

#undef DeclareStageCallback
