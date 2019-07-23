/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <StageAdapter.h>

#define DeclareStageCallback(NAME) \
    public: Napi::Value Get_##NAME(const Napi::CallbackInfo& info) { return this->callbacks[NAME].Value(); } \
    public: void Set_##NAME(const Napi::CallbackInfo& info, const Napi::Value& value) \
        { this->SetCallback(&this->callbacks[NAME], value); }

namespace ls {

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
        onGamepadHatMotion,
        onQuit,
        StageCallbacksCount,
    };

 public:
    explicit SDLStageAdapter(const Napi::CallbackInfo& info);
    virtual ~SDLStageAdapter() = default;

    static Napi::Function Constructor(Napi::Env env);
    Napi::Value ProcessEvents(const Napi::CallbackInfo& info) override;
    Napi::Value CreateSceneAdapter(const Napi::CallbackInfo& info) override;
    void ResetCallbacks(const Napi::CallbackInfo& info);

    DeclareStageCallback(onGamepadConnected);
    DeclareStageCallback(onGamepadDisconnected);
    DeclareStageCallback(onKeyboardKeyUp);
    DeclareStageCallback(onKeyboardKeyDown);
    DeclareStageCallback(onGamepadButtonUp);
    DeclareStageCallback(onGamepadButtonDown);
    DeclareStageCallback(onGamepadAxisMotion);
    DeclareStageCallback(onGamepadHatMotion);
    DeclareStageCallback(onQuit);

 private:
    static constexpr int NUM_EVENTS_PER_FRAME{20};
    void SetCallback(Napi::FunctionReference* function, const Napi::Value& value);
    inline void Call(const StageCallbacks callbackId, const std::initializer_list<napi_value>& args);
    inline bool IsCallbackEmpty(const StageCallbacks callbackId);

 private:
    Napi::FunctionReference callbacks[StageCallbacksCount];
};

} // namespace ls

#undef DeclareStageCallback
