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
#include <ls/Log.h>
#include <ls/Format.h>
#include <ls/Timer.h>
#include <ls/CapabilitiesView.h>

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
using Napi::Persistent;
using Napi::SafeObjectWrap;
using Napi::QueryInterface;
using Napi::String;
using Napi::Value;

bool operator == (SDL_DisplayMode const& lhs, SDL_DisplayMode const& rhs) {
    return (lhs.w == rhs.w) && (lhs.h == rhs.h);
}

struct SDL_DisplayModeHash {
    std::size_t operator()(const SDL_DisplayMode& displayMode) const noexcept {
        return (static_cast<std::size_t>(displayMode.w) << 32) | static_cast<std::size_t>(displayMode.h);
    }
};

namespace ls {

// Note, SDL_JOYSTICK_AXIS_* defines were introduced after 2.0.4.

constexpr auto SDL_JOYSTICK_AXIS_MIN_F =
#ifndef SDL_JOYSTICK_AXIS_MIN
     -32768.f;
#else
    static_cast<float>(SDL_JOYSTICK_AXIS_MIN);
#endif

constexpr auto SDL_JOYSTICK_AXIS_MAX_F =
#ifndef SDL_JOYSTICK_AXIS_MAX
    32767.f;
#else
    static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
#endif

std::unordered_map<std::string, SDLStageAdapter::StageCallback> SDLStageAdapter::callbackMap{
    { "connected", StageCallbackGamepadConnected },
    { "disconnected", StageCallbackGamepadDisconnected },
    { "keyup", StageCallbackKeyboardKeyUp },
    { "keydown", StageCallbackKeyboardKeyDown },
    { "buttonup", StageCallbackGamepadButtonUp },
    { "buttondown", StageCallbackGamepadButtonDown },
    { "hatup", StageCallbackHatUp },
    { "hatdown", StageCallbackHatDown },
    { "axismotion", StageCallbackGamepadAxisMotion },
    { "quit", StageCallbackQuit },
};

SDLStageAdapter::SDLStageAdapter(const CallbackInfo& info) : SafeObjectWrap<SDLStageAdapter>(info) {
}

void SDLStageAdapter::Constructor(const Napi::CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    SDL_version compiled{};
    SDL_version linked{};

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    LOG_INFO("SDL Version: compiled=%i.%i.%i linked=%i.%i.%i",
             compiled.major, compiled.minor, compiled.patch,
             linked.major, linked.minor, linked.patch);

    this->Init();
    this->capabilities = this->DetermineCapabilities();
    this->capabilitiesRef = Persistent(ToCapabilitiesView(env, capabilities));
}

Function SDLStageAdapter::GetClass(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        constructor = DefineClass(env, "SDLStageAdapter", true, {
            StaticValue("type", String::New(env, "graphics"), napi_default),
            InstanceMethod("getKeyboard", &SDLStageAdapter::GetKeyboard),
            InstanceMethod("getGamepads", &SDLStageAdapter::GetGamepads),
            InstanceAccessor("capabilities", &SDLStageAdapter::GetCapabilities, nullptr),
            InstanceMethod("createSceneAdapter", &SDLStageAdapter::CreateSceneAdapter),
            InstanceMethod("processEvents", &SDLStageAdapter::ProcessEvents),
            InstanceMethod("attach", &SDLStageAdapter::Attach),
            InstanceMethod("detach", &SDLStageAdapter::Detach),
            InstanceMethod("destroy", &SDLStageAdapter::Destroy),
            InstanceMethod("setCallback", &SDLStageAdapter::SetCallback),
            InstanceMethod("resetCallbacks", &SDLStageAdapter::ResetCallbacks),
            InstanceMethod("addGameControllerMappings", &SDLStageAdapter::AddGameControllerMappings),
        });
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

Value SDLStageAdapter::GetCapabilities(const Napi::CallbackInfo &info) {
    return this->capabilitiesRef.Value();
}

Capabilities SDLStageAdapter::DetermineCapabilities() {
    auto env{ this->Env() };
    Capabilities caps{};
    auto numVideoDisplays{ SDL_GetNumVideoDisplays() };

    if (numVideoDisplays < 0) {
        throw Error::New(env, Format("Failed to get display count. SDL Error: %s", SDL_GetError()));
    }

    for (auto i{ 0 }; i < numVideoDisplays; i++) {
      Display display{};
      auto name{ SDL_GetDisplayName(i) };

      display.name = name ? name : "";

      auto numDisplayModes{ SDL_GetNumDisplayModes(i) };

      if (numDisplayModes < 0) {
        throw Error::New(env, Format("Failed to get display count. SDL Error: %s", SDL_GetError()));
      }

      SDL_DisplayMode desktopDisplayMode;

      if (SDL_GetDesktopDisplayMode(i, &desktopDisplayMode) != 0) {
        throw Error::New(env, Format("Failed to get default display mode. SDL Error: %s", SDL_GetError()));
      }

      display.defaultMode = { desktopDisplayMode.w, desktopDisplayMode.h };

      std::unordered_set<SDL_DisplayMode, SDL_DisplayModeHash> uniqueDisplayModes(numDisplayModes);

      for (auto j{ 0 }; j < numDisplayModes; j++) {
        SDL_DisplayMode displayMode;

        if (SDL_GetDisplayMode(i, j, &displayMode) != 0) {
          throw Error::New(env, Format("Failed to get display mode. SDL Error: %s", SDL_GetError()));
        }

        uniqueDisplayModes.insert(displayMode);
      }

      for (auto& p : uniqueDisplayModes) {
        display.modes.push_back({ p.w, p.h });
      }

      caps.displays.emplace_back(display);
    }

    return caps;
}

Value SDLStageAdapter::AddGameControllerMappings(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto file{ info[0].As<String>().Utf8Value() };
    auto result{ SDL_GameControllerAddMapping(file.c_str()) };

    if (result == -1) {
        throw Error::New(env, Format("addGameControllerMappings(): %s", SDL_GetError()));
    }

    return Number::New(env, result);
}

void SDLStageAdapter::SetCallback(const CallbackInfo& info) {
    auto env{ info.Env() };
    auto id{ info[0].As<String>().Utf8Value() };
    auto it{ callbackMap.find(info[0].As<String>().Utf8Value()) };

    if (it == callbackMap.end()) {
        throw Error::New(env, Format("Unknown callback id: %s", id));
    }

    if (info[1].IsFunction()) {
        this->callbacks[it->second].Reset(info[1].As<Function>(), 1);
    } else {
        this->callbacks[it->second].Reset();
    }
}

void SDLStageAdapter::ResetCallbacks(const CallbackInfo& info) {
    for (auto& callback : this->callbacks) {
        callback.Reset();
    }
}

void SDLStageAdapter::Init() {
    auto env{ this->Env() };
    Timer t("StageAdapter.attach()");

    if (SDL_WasInit(SDL_INIT_VIDEO) == 0 && SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw Error::New(env, Format("Failed to init SDL video. SDL Error: %s", SDL_GetError()));
    }

    t.Log();

    if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0 && SDL_Init(SDL_INIT_JOYSTICK) != 0) {
        throw Error::New(env, Format("Failed to init SDL joystick. SDL Error: %s", SDL_GetError()));
    }

    t.Log();

    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0 && SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        throw Error::New(env, Format("Failed to init SDL gamecontroller. SDL Error: %s", SDL_GetError()));
    }

    SDL_GameControllerEventState(SDL_IGNORE);

    t.Log();
}

void SDLStageAdapter::Attach(const CallbackInfo& info) {
    auto env{ info.Env() };

    if (this->isAttached) {
        return;
    }

    this->isAttached = false;

    this->Init();

    this->SyncGamepads(env);

    if (!this->keyboard) {
        this->keyboard = QueryInterface<SDLKeyboard>(SDLKeyboard::GetClass(env).New({}));
        this->keyboard->Ref();
    }

    this->isAttached = true;
}

void SDLStageAdapter::Detach(const CallbackInfo& info) {
    if (!this->isAttached) {
        return;
    }

    this->ClearGamepads();

    SDL_Quit();

    this->isAttached = false;
}

void SDLStageAdapter::Destroy(const CallbackInfo& info) {
    this->ResetCallbacks(info);

    if (this->keyboard) {
        this->keyboard->Unref();
        this->keyboard = nullptr;
    }

    this->Detach(info);
}

Value SDLStageAdapter::CreateSceneAdapter(const Napi::CallbackInfo& info) {
    return info.Env().Null();
}

//std::unique_ptr<SceneAdapter> SDLStageAdapter::CreateSceneAdapter(const SceneAdapterConfig& config) {
//    return std::unique_ptr<SceneAdapter>(new SDLSceneAdapter(config));
//}

Value SDLStageAdapter::ProcessEvents(const Napi::CallbackInfo& info) {
    static constexpr auto NUM_EVENTS_PER_FRAME{ 20 };
    static SDL_Event eventBuffer[NUM_EVENTS_PER_FRAME];

    SDL_PumpEvents();

    auto env{ info.Env() };
    auto eventIndex{ 0 };
    auto eventCount{ SDL_PeepEvents(eventBuffer, NUM_EVENTS_PER_FRAME, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) };
    auto result{ true };

    if (eventCount < 0) {
        LOG_ERROR(SDL_GetError());
        result = false;
    }

    while (result && eventIndex < eventCount) {
        if (!this->isAttached) {
            break;
        }

        const auto& event{ eventBuffer[eventIndex++] };

        switch (event.type) {
            case SDL_QUIT:
                result = this->DispatchQuit(env);
                break;
            case SDL_KEYUP:
                this->DispatchKeyboardKeyUp(env, event.key.keysym.scancode);
                break;
            case SDL_KEYDOWN:
                this->DispatchKeyboardKeyDown(env, event.key.keysym.scancode, event.key.repeat != 0);
                break;
            case SDL_JOYBUTTONUP:
                this->DispatchJoystickButtonUp(env, event.jbutton.which, event.jbutton.button);
                break;
            case SDL_JOYBUTTONDOWN:
                this->DispatchJoystickButtonDown(env, event.jbutton.which, event.jbutton.button);
                break;
            case SDL_JOYHATMOTION:
                this->DispatchJoystickHatMotion(env, event.jhat.which, event.jhat.hat, event.jhat.value);
                break;
            case SDL_JOYAXISMOTION:
                this->DispatchJotstickAxisMotion(
                    env,
                    event.jaxis.which,
                    event.jaxis.axis,
                    static_cast<float>(event.jaxis.value));
                break;
            case SDL_JOYDEVICEADDED:
                this->DispatchJoystickAdded(env, event.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                this->DispatchJoystickRemoved(env, event.jdevice.which);
                break;
            default:
                break;
        }
    }

    return Boolean::New(env, result);
}

bool SDLStageAdapter::DispatchQuit(Napi::Env env) {
    if (!this->IsCallbackEmpty(StageCallbackQuit)) {
        HandleScope scope(env);

        this->Call(StageCallbackQuit, {});
    }

    // exit on quit
    // TODO: allow user to override
    return false;
}

void SDLStageAdapter::DispatchKeyboardKeyDown(Napi::Env env, int32_t scanCode, bool isRepeat) {
    if (!this->IsCallbackEmpty(StageCallbackKeyboardKeyDown)) {
        HandleScope scope(env);

        this->Call(StageCallbackKeyboardKeyDown, {
            this->keyboard->Value(),
            Number::New(env, scanCode),
            Boolean::New(env, isRepeat),
        });
    }
}

void SDLStageAdapter::DispatchKeyboardKeyUp(Napi::Env env, int32_t scanCode) {
    if (!this->IsCallbackEmpty(StageCallbackKeyboardKeyUp)) {
        HandleScope scope(env);

        this->Call(StageCallbackKeyboardKeyUp, {
            this->keyboard->Value(),
            Number::New(env, scanCode)
        });
    }
}

void SDLStageAdapter::DispatchJoystickButtonUp(Napi::Env env, int32_t instanceId, int32_t buttonId) {
    if (!this->IsCallbackEmpty(StageCallbackGamepadButtonUp)) {
        HandleScope scope(env);

        this->Call(StageCallbackGamepadButtonUp, {
            this->GetGamepad(env, instanceId),
            Number::New(env, buttonId),
        });
    }
}

void SDLStageAdapter::DispatchJoystickButtonDown(Napi::Env env, int32_t instanceId, int32_t buttonId) {
    if (!this->IsCallbackEmpty(StageCallbackGamepadButtonDown)) {
        HandleScope scope(env);

        this->Call(StageCallbackGamepadButtonDown, {
            this->GetGamepad(env, instanceId),
            Number::New(env, buttonId),
        });
    }
}

void SDLStageAdapter::DispatchJotstickAxisMotion(Napi::Env env, int32_t instanceId, uint8_t axisIndex, float value) {
    if (!this->IsCallbackEmpty(StageCallbackGamepadAxisMotion)) {
        HandleScope scope(env);

        this->Call(StageCallbackGamepadAxisMotion, {
            this->GetGamepad(env, instanceId),
            Number::New(env, axisIndex),
            Number::New(env, value < 0 ? -(value / SDL_JOYSTICK_AXIS_MIN_F) : value / SDL_JOYSTICK_AXIS_MAX_F),
        });
    }
}

void SDLStageAdapter::DispatchJoystickHatMotion(Napi::Env env, int32_t instanceId, uint8_t hatIndex, uint8_t hatValue) {
    auto p{ this->gamepadsByInstanceId.find(instanceId) };

    if (p == this->gamepadsByInstanceId.end()) {
        return;
    }

    auto gamepad{ p->second };
    auto state{ gamepad->GetHatState(hatIndex) };
    HandleScope scope(env);

    if (state != 0) {
        gamepad->SetHatState(hatIndex, 0);
        if (!IsCallbackEmpty(StageCallbackHatUp)) {
            Call(StageCallbackHatUp, {
                gamepad->Value(),
                Number::New(env, hatIndex),
                Number::New(env, state),
            });
        }
    }

    switch (hatValue) {
        case SDL_HAT_UP:
        case SDL_HAT_RIGHT:
        case SDL_HAT_DOWN:
        case SDL_HAT_LEFT:
            gamepad->SetHatState(hatIndex, hatValue);
            if (!IsCallbackEmpty(StageCallbackHatDown)) {
                Call(StageCallbackHatDown, {
                    gamepad->Value(),
                    Number::New(env, hatIndex),
                    Number::New(env, hatValue),
                });
            }
            break;
        default:
            break;
    }
}

void SDLStageAdapter::DispatchJoystickAdded(Napi::Env env, int32_t index) {
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

    // TODO: exception
    auto gamepad{ this->AddGamepad(env, index) };

    if (!IsCallbackEmpty(StageCallbackGamepadConnected)) {
        HandleScope scope(env);

        Call(StageCallbackGamepadConnected, { gamepad->Value() });
    }
}

void SDLStageAdapter::DispatchJoystickRemoved(Napi::Env env, int32_t instanceId) {
    auto p{ this->gamepadsByInstanceId.find(instanceId) };

    if (p == this->gamepadsByInstanceId.end()) {
        return;
    }

    auto gamepad{ p->second };

    this->gamepadsByInstanceId.erase(p);

    if (!IsCallbackEmpty(StageCallbackGamepadDisconnected)) {
        HandleScope scope(env);

        Call(StageCallbackGamepadDisconnected, { gamepad->Value() });
    }

    gamepad->Destroy();
    gamepad->Unref();
}

void SDLStageAdapter::Call(const StageCallback callbackId, const std::initializer_list<napi_value>& args) {
    try {
        this->callbacks[callbackId](args);
    } catch (const std::exception& e) {
        LOG_ERROR("Callback (%i) uncaught JS exception: %s", callbackId, e);
    }
}

bool SDLStageAdapter::IsCallbackEmpty(const StageCallback callbackId) {
    return this->callbacks[callbackId].IsEmpty();
}

void SDLStageAdapter::SyncGamepads(Napi::Env env) {
    this->ClearGamepads();

    for (int32_t i{ 0 }; i < SDL_NumJoysticks(); i++) {
        try {
            this->AddGamepad(env, i);
        } catch (const std::exception& e) {
            LOG_ERROR("Joystick #%i: %s", i, e);
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
    auto gamepad{ SDLGamepad::New(env, index) };

    gamepad->Ref();

    try {
        this->gamepadsByInstanceId[gamepad->GetId()] = gamepad;
    } catch (const std::exception&) {
        gamepad->Unref();
    }

    return gamepad;
}

Value SDLStageAdapter::GetGamepad(Napi::Env env, int32_t instanceId) {
    auto p{ this->gamepadsByInstanceId.find(instanceId) };

    return p == this->gamepadsByInstanceId.end() ? env.Undefined() : p->second->Value();
}

} // namespace ls

#undef CallbackInstanceAccessor
