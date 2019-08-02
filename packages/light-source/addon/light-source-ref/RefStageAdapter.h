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

class RefStageAdapter : public StageAdapter, public Napi::ObjectWrap<RefStageAdapter> {
 public:
    explicit RefStageAdapter(const Napi::CallbackInfo& info);
    virtual ~RefStageAdapter() = default;

    static Napi::Function Constructor(Napi::Env env);
    Napi::Value GetKeyboard(const Napi::CallbackInfo& info);
    Napi::Value GetGamepads(const Napi::CallbackInfo& info);
    Napi::Value GetDisplays(const Napi::CallbackInfo& info);
    Napi::Value ProcessEvents(const Napi::CallbackInfo& info);
    void Attach(const Napi::CallbackInfo& info);
    void Detach(const Napi::CallbackInfo& info);
    void ResetCallbacks(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);

    std::unique_ptr<SceneAdapter> CreateSceneAdapter(const SceneAdapterConfig& config) override;

// private:
//    void SetCallback(Napi::FunctionReference* function, const Napi::Value& value);
//    inline void Call(const StageCallbacks callbackId, const std::initializer_list<napi_value>& args);
//    inline bool IsCallbackEmpty(const StageCallbacks callbackId);
//    void SyncGamepads(Napi::Env env);
//    void ClearGamepads();
//    SDLGamepad* AddGamepad(Napi::Env env, int32_t index);
//    inline Napi::Value GetGamepad(Napi::Env env, int32_t instanceId);
//    void HandleJoystickHatMotion(Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue);
//    void HandleJoystickAdded(Napi::Env env, int32_t index);
//    void HandleJoystickRemoved(Napi::Env env, int32_t instanceId);
//    void Attach(Napi::Env env);
//    void Detach(Napi::Env env);
};

} // namespace ls

#undef DeclareStageCallback
