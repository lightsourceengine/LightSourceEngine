/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLStageAdapter.h"
#include "SDLSceneAdapter.h"
#include "SDLKeyboard.h"
#include "SDLGamepad.h"
#include <unordered_set>
#include <fmt/format.h>

using Napi::Array;
using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

#define CallbackInstanceAccessor(NAME) \
    InstanceAccessor(#NAME, &SDLStageAdapter::Get_##NAME, &SDLStageAdapter::Set_##NAME)

inline
bool operator == (SDL_DisplayMode const& lhs, SDL_DisplayMode const& rhs) {
    return (lhs.w == rhs.w) && (lhs.h == rhs.h);
}

namespace std {

template<> struct hash<SDL_DisplayMode> {
    std::size_t operator()(const SDL_DisplayMode& displayMode) const noexcept{
        return (static_cast<std::size_t>(displayMode.w) << 32) | static_cast<std::size_t>(displayMode.h);
    }
};

} // namespace std

namespace ls {

Object ToDisplayObject(Napi::Env env, int32_t displayIndex);
Object ToDisplayModeObject(Napi::Env env, const SDL_DisplayMode& mode);

constexpr auto SDL_JOYSTICK_AXIS_MIN_F = static_cast<float>(SDL_JOYSTICK_AXIS_MIN);
constexpr auto SDL_JOYSTICK_AXIS_MAX_F = static_cast<float>(SDL_JOYSTICK_AXIS_MAX);

SDLStageAdapter::SDLStageAdapter(const CallbackInfo& info) : ObjectWrap<SDLStageAdapter>(info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    SDL_version compiled{};
    SDL_version linked{};

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    fmt::println("SDL Version: compiled={}.{}.{} linked={}.{}.{}",
        compiled.major, compiled.minor, compiled.patch,
        linked.major, linked.minor, linked.patch);

    this->Attach(env);

    this->keyboard = ObjectWrap<SDLKeyboard>::Unwrap(SDLKeyboard::Constructor(env).New({}));
    this->keyboard->Ref();
}

Function SDLStageAdapter::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SDLStageAdapter", {
            InstanceMethod("getKeyboard", &SDLStageAdapter::GetKeyboard),
            InstanceMethod("getGamepads", &SDLStageAdapter::GetGamepads),
            InstanceMethod("getDisplays", &SDLStageAdapter::GetDisplays),
            InstanceMethod("resetCallbacks", &SDLStageAdapter::ResetCallbacks),
            InstanceMethod("processEvents", &SDLStageAdapter::ProcessEvents),
            InstanceMethod("attach", &SDLStageAdapter::Attach),
            InstanceMethod("detach", &SDLStageAdapter::Detach),
            InstanceMethod("destroy", &SDLStageAdapter::Destroy),
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

Value SDLStageAdapter::GetDisplays(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto displayCount{ SDL_GetNumVideoDisplays() };

    if (displayCount < 0) {
        throw Error::New(env, fmt::format("Failed to get display count. SDL Error: {}", SDL_GetError()));
    }

    auto displayArray{ Array::New(env, displayCount) };

    for (auto i{ 0 }; i < displayCount; i++) {
        displayArray.Set(static_cast<uint32_t>(i), ToDisplayObject(env, i));
    }

    return displayArray;
}

void SDLStageAdapter::Attach(const CallbackInfo& info) {
    this->Attach(info.Env());
}

void SDLStageAdapter::Attach(Napi::Env env) {
    if (this->isAttached) {
        return;
    }

    HandleScope scope(env);

    if (SDL_WasInit(SDL_INIT_VIDEO) == 0 && SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw Error::New(env, fmt::format("Failed to init SDL video. SDL Error: {}", SDL_GetError()));
    }

    if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0 && SDL_Init(SDL_INIT_JOYSTICK) != 0) {
        throw Error::New(env, fmt::format("Failed to init SDL joystick. SDL Error: {}", SDL_GetError()));
    }

    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0 && SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        throw Error::New(env, fmt::format("Failed to init SDL gamecontroller. SDL Error: {}", SDL_GetError()));
    }

    SDL_GameControllerEventState(SDL_IGNORE);
    this->SyncGamepads(env);

    this->isAttached = true;
}

void SDLStageAdapter::Detach(const CallbackInfo& info) {
    this->Detach(info.Env());
}

void SDLStageAdapter::Detach(Napi::Env env) {
    this->ClearGamepads();
    this->isAttached = false;
}

void SDLStageAdapter::Destroy(const CallbackInfo& info) {
    for (auto& ref : callbacks) {
        ref.Reset();
    }

    if (this->keyboard) {
        this->keyboard->Unref();
        this->keyboard = nullptr;
    }

    this->ClearGamepads();
    this->gamepadsByInstanceId.clear();
}

std::unique_ptr<SceneAdapter> SDLStageAdapter::CreateSceneAdapter(const SceneAdapterConfig& config) {
    return std::make_unique<SDLSceneAdapter>(config);
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
    this->ClearGamepads();

    for (int32_t i{ 0 }; i < SDL_NumJoysticks(); i++) {
        try {
            this->AddGamepad(env, i);
        } catch (std::exception& e) {
            fmt::println("Error: Failed to get joystick at index {}", i);
        }
    }
}

void SDLStageAdapter::ClearGamepads() {
    for (auto& p : this->gamepadsByInstanceId) {
        p.second->Destroy();
        p.second->Unref();
    }

    this->gamepadsByInstanceId.clear();
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

Object ToDisplayObject(Napi::Env env, int32_t displayIndex) {
    auto displayObject{ Object::New(env) };
    auto name{ SDL_GetDisplayName(displayIndex) };

    displayObject["name"] = String::New(env, name ? name : "");

    auto displayModeCount{ SDL_GetNumDisplayModes(displayIndex) };

    if (displayModeCount < 0) {
        throw Error::New(env, fmt::format("Failed to get display count. SDL Error: {}", SDL_GetError()));
    }

    SDL_DisplayMode current;

    if (SDL_GetDesktopDisplayMode(displayIndex, &current) != 0) {
        throw Error::New(env, fmt::format("Failed to get default display mode. SDL Error: {}", SDL_GetError()));
    }

    displayObject["defaultMode"] = ToDisplayModeObject(env, current);

    std::unordered_set<SDL_DisplayMode> uniqueDisplayModes;

    for (auto j{ 0 }; j < displayModeCount; j++) {
        SDL_DisplayMode displayMode;

        if (SDL_GetDisplayMode(displayIndex, j, &displayMode) != 0) {
            throw Error::New(env,
                fmt::format("Failed to get display mode. SDL Error: {}", SDL_GetError()));
        }

        uniqueDisplayModes.insert(displayMode);
    }

    auto displayModeArray{ Array::New(env, uniqueDisplayModes.size()) };
    auto k{ 0u };

    for (auto& p : uniqueDisplayModes) {
        displayModeArray.Set(k++, ToDisplayModeObject(env, p));
    }

    displayObject["modes"] = displayModeArray;

    return displayObject;
}

Object ToDisplayModeObject(Napi::Env env, const SDL_DisplayMode& mode) {
    auto result{ Object::New(env) };

    result["width"] = Number::New(env, mode.w);
    result["height"] = Number::New(env, mode.h);

    return result;
}

} // namespace ls

#undef CallbackInstanceAccessor
