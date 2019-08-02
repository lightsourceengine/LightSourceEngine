/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>
#include <napi.h>
#include <StageAdapter.h>

#define DeclareStageCallback(NAME) \
    public: Napi::Value Get_##NAME(const Napi::CallbackInfo& info) { return this->callbacks[NAME].Value(); } \
    public: void Set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) \
        { this->SetCallback(&this->callbacks[NAME], value); }

namespace ls {

class SDLKeyboard;
class SDLGamepad;

class SDLStageAdapter : public StageAdapter, public Napi::ObjectWrap<SDLStageAdapter> {
 private:
    enum StageCallbacks {
        onGamepadConnected,
        onGamepadDisconnected,
        onKeyboardKeyUp,
        onKeyboardKeyDown,
        onGamepadButtonUp,
        onGamepadButtonDown,
        onGamepadAxisMotion,
        onQuit,
        StageCallbacksCount,
    };

 public:
    explicit SDLStageAdapter(const Napi::CallbackInfo& info);
    virtual ~SDLStageAdapter() = default;

    static Napi::Function Constructor(Napi::Env env);
    Napi::Value GetKeyboard(const Napi::CallbackInfo& info);
    Napi::Value GetGamepads(const Napi::CallbackInfo& info);
    Napi::Value GetDisplays(const Napi::CallbackInfo& info);
    Napi::Value ProcessEvents(const Napi::CallbackInfo& info);
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void ResetCallbacks(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);

    DeclareStageCallback(onGamepadConnected);
    DeclareStageCallback(onGamepadDisconnected);
    DeclareStageCallback(onKeyboardKeyUp);
    DeclareStageCallback(onKeyboardKeyDown);
    DeclareStageCallback(onGamepadButtonUp);
    DeclareStageCallback(onGamepadButtonDown);
    DeclareStageCallback(onGamepadAxisMotion);
    DeclareStageCallback(onQuit);

    void ProcessEvents() override;
    std::unique_ptr<SceneAdapter> CreateSceneAdapter(const SceneAdapterConfig& config) override;

 private:
    void SetCallback(Napi::FunctionReference* function, const Napi::Value& value);
    inline void Call(const StageCallbacks callbackId, const std::initializer_list<napi_value>& args);
    inline bool IsCallbackEmpty(const StageCallbacks callbackId);
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

    Napi::FunctionReference callbacks[StageCallbacksCount];
    SDLKeyboard* keyboard{};
    std::unordered_map<int32_t, SDLGamepad*> gamepadsByInstanceId{};
    bool isAttached{false};
};

} // namespace ls

#undef DeclareStageCallback
