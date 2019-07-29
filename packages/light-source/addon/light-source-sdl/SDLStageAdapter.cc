/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLStageAdapter.h"
#include "SDLSceneAdapter.h"
#include "SDLKeyboard.h"
#include "SDLGamepad.h"
#include <fmt/format.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::Value;

#define CallbackInstanceAccessor(NAME) \
    InstanceAccessor(#NAME, &SDLStageAdapter::Get_##NAME, &SDLStageAdapter::Set_##NAME)

namespace ls {

constexpr auto SDL_JOYSTICK_AXIS_MIN_F = static_cast<float>(SDL_JOYSTICK_AXIS_MIN);
constexpr auto SDL_JOYSTICK_AXIS_MAX_F = static_cast<float>(SDL_JOYSTICK_AXIS_MAX);

SDLStageAdapter::SDLStageAdapter(const CallbackInfo& info) : ObjectWrap<SDLStageAdapter>(info) {
    auto env{ info.Env() };
    HandleScope scope(env);
    auto keyboardObject{ SDLKeyboard::Constructor(env).New({}) };

    this->keyboard = ObjectWrap<SDLKeyboard>::Unwrap(keyboardObject);
    this->keyboard->Ref();

    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
    SDL_GameControllerEventState(SDL_IGNORE);
    this->SyncGamepads(env);
}

Function SDLStageAdapter::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SDLStageAdapter", {
            InstanceAccessor("keyboard", &SDLStageAdapter::GetKeyboard, nullptr),
            InstanceMethod("getGamepads", &SDLStageAdapter::GetGamepads),
            InstanceMethod("resetCallbacks", &SDLStageAdapter::ResetCallbacks),
            InstanceMethod("processEvents", &SDLStageAdapter::ProcessEvents),
            CallbackInstanceAccessor(onQuit),
            CallbackInstanceAccessor(onKeyboardKeyUp),
            CallbackInstanceAccessor(onKeyboardKeyDown),
            CallbackInstanceAccessor(onGamepadButtonDown),
            CallbackInstanceAccessor(onGamepadButtonUp),
            CallbackInstanceAccessor(onGamepadAxisMotion),
            CallbackInstanceAccessor(onGamepadConnected),
            CallbackInstanceAccessor(onGamepadDisconnected),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value SDLStageAdapter::GetKeyboard(const CallbackInfo& info) {
    return this->keyboard->Value();
}

Value SDLStageAdapter::GetGamepads(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);
    auto array{ Array::New(env, this->gamepadsByInstanceId.size()) };
    auto index{ 0u };

    for (auto& p : this->gamepadsByInstanceId) {
        array.Set(index++, p.second->Value());
    }

    return scope.Escape(array);
}

void SDLStageAdapter::ProcessEvents() {
}

std::shared_ptr<SceneAdapter> SDLStageAdapter::CreateSceneAdapter(int32_t displayId) {
    return std::static_pointer_cast<SceneAdapter>(std::make_shared<SDLSceneAdapter>(displayId));
}

Value SDLStageAdapter::ProcessEvents(const CallbackInfo& info) {
    static SDL_Event eventBuffer[NUM_EVENTS_PER_FRAME];

    SDL_PumpEvents();

    auto env{ info.Env() };
    auto eventIndex{ 0 };
    auto eventCount{ SDL_PeepEvents(eventBuffer, NUM_EVENTS_PER_FRAME, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) };

    if (eventCount < 0) {
        return Boolean::New(env, false);
    }

    while (eventIndex < eventCount) {
        HandleScope scope(env);
        float value;
        const auto& event = eventBuffer[eventIndex++];

        switch (event.type) {
            case SDL_QUIT:
                if (!IsCallbackEmpty(onQuit)) {
                    Call(onQuit, {});
                }
                // TODO: user override?
                return Boolean::New(env, false);
            case SDL_KEYUP:
                if (!IsCallbackEmpty(onKeyboardKeyUp)) {
                    Call(onKeyboardKeyUp, {
                        this->keyboard->Value(),
                        Number::New(env, event.key.keysym.scancode)
                    });
                }
                break;
            case SDL_KEYDOWN:
                if (!IsCallbackEmpty(onKeyboardKeyDown)) {
                    Call(onKeyboardKeyDown, {
                        this->keyboard->Value(),
                        Number::New(env, event.key.keysym.scancode),
                        Number::New(env, event.key.repeat != 0),
                    });
                }
                break;
            case SDL_JOYBUTTONUP:
                if (!IsCallbackEmpty(onGamepadButtonUp)) {
                    Call(onGamepadButtonUp, {
                        this->GetGamepad(env, event.jbutton.which),
                        Number::New(env, event.jbutton.button),
                    });
                }
                break;
            case SDL_JOYBUTTONDOWN:
                if (!IsCallbackEmpty(onGamepadButtonDown)) {
                    Call(onGamepadButtonDown, {
                        this->GetGamepad(env, event.jbutton.which),
                        Number::New(env, event.jbutton.button),
                    });
                }
                break;
            case SDL_JOYHATMOTION:
                HandleJoystickHatMotion(env, event.jhat.which, event.jhat.hat, event.jhat.value);
                break;
            case SDL_JOYAXISMOTION:
                value = static_cast<float>(event.jaxis.value);

                if (!IsCallbackEmpty(onGamepadAxisMotion)) {
                    Call(onGamepadAxisMotion, {
                        this->GetGamepad(env, event.jaxis.which),
                        Number::New(env, event.jaxis.axis),
                        Number::New(env, value < 0 ? -SDL_JOYSTICK_AXIS_MIN_F : SDL_JOYSTICK_AXIS_MAX_F),
                    });
                }
                break;
            case SDL_JOYDEVICEADDED:
                HandleJoystickAdded(env, event.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                HandleJoystickRemoved(env, event.jdevice.which);
                break;
            default:
                break;
        }
    }

    return Boolean::New(env, true);
}

void SDLStageAdapter::HandleJoystickHatMotion(Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue) {
    auto p{ this->gamepadsByInstanceId.find(instanceId) };

    if (p == this->gamepadsByInstanceId.end()) {
        return;
    }

    auto gamepad{ p->second };
    auto state{ gamepad->GetHatState(hatIndex) };
    int32_t button;

    if (state != 0) {
        button = gamepad->GetHatButtonIndex(hatIndex, state);
        gamepad->SetHatState(hatIndex, 0);

        if (!IsCallbackEmpty(onGamepadButtonUp)) {
            Call(onGamepadButtonUp, {
                gamepad->Value(),
                Number::New(env, button),
            });
        }
    }

    if ((button = gamepad->GetHatButtonIndex(hatIndex, hatValue)) != -1) {
        gamepad->SetHatState(hatIndex, hatValue);

        if (!IsCallbackEmpty(onGamepadButtonDown)) {
            Call(onGamepadButtonDown, {
                gamepad->Value(),
                Number::New(env, button),
            });
        }
    }
}

void SDLStageAdapter::HandleJoystickAdded(Napi::Env env, int32_t index) {
    auto joystick{ SDL_JoystickOpen(index) };

    if (!joystick) {
        // TODO: ????
        return;
    }

    auto instanceId{ SDL_JoystickInstanceID(joystick) };

    SDL_JoystickClose(joystick);

    if (this->gamepadsByInstanceId.find(instanceId) != this->gamepadsByInstanceId.end()) {
        return;
    }

    auto gamepad{ this->AddGamepad(env, index) };

    if (!IsCallbackEmpty(onGamepadConnected)) {
        Call(onGamepadConnected, { gamepad->Value() });
    }
}

void SDLStageAdapter::HandleJoystickRemoved(Napi::Env env, int32_t instanceId) {
    auto p{ this->gamepadsByInstanceId.find(instanceId) };

    if (p == this->gamepadsByInstanceId.end()) {
        return;
    }

    auto gamepad{ p->second };

    this->gamepadsByInstanceId.erase(p);

    if (!IsCallbackEmpty(onGamepadDisconnected)) {
        Call(onGamepadDisconnected, { gamepad->Value() });
    }

    gamepad->Destroy();
    gamepad->Unref();
}

void SDLStageAdapter::ResetCallbacks(const CallbackInfo& info) {
    for (int i{ 0 }; i < StageCallbacksCount; i++) {
        this->callbacks[i].Reset();
    }
}

void SDLStageAdapter::SetCallback(FunctionReference* function, const Napi::Value& value) {
    if (value.IsFunction()) {
        function->Reset(value.As<Function>(), 1);
    } else {
        function->Reset();
    }

    // TODO: throw if not null or undefined?
}

inline
void SDLStageAdapter::Call(const StageCallbacks callbackId, const std::initializer_list<napi_value>& args) {
    try {
        this->callbacks[callbackId](args);
    } catch (Napi::Error& error) {
         fmt::println("SDLStageAdapter: Uncaught exception calling JS event handler: {}", error.what());
    }
}

inline
bool SDLStageAdapter::IsCallbackEmpty(const StageCallbacks callbackId) {
    return this->callbacks[callbackId].IsEmpty();
}

void SDLStageAdapter::SyncGamepads(Napi::Env env) {
    for (auto& p : this->gamepadsByInstanceId) {
        p.second->Destroy();
        p.second->Unref();
    }

    this->gamepadsByInstanceId.clear();

    for (int32_t i{ 0 }; i < SDL_NumJoysticks(); i++) {
        this->AddGamepad(env, i);
    }
}

SDLGamepad* SDLStageAdapter::AddGamepad(Napi::Env env, int32_t index) {
    HandleScope scope(env);
    auto gamepadObject{ SDLGamepad::Constructor(env).New({ Number::New(env, index) }) };
    auto gamepad{ ObjectWrap<SDLGamepad>::Unwrap(gamepadObject) };

    gamepad->Ref();

    this->gamepadsByInstanceId[gamepad->GetId()] = gamepad;

    return gamepad;
}

inline
Value SDLStageAdapter::GetGamepad(Napi::Env env, int32_t instanceId) {
    auto p{ this->gamepadsByInstanceId.find(instanceId) };

    return p == this->gamepadsByInstanceId.end() ? env.Undefined() : p->second->Value();
}

} // namespace ls

#undef CallbackInstanceAccessor
