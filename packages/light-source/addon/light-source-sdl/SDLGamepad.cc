/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLGamepad.h"
#include <fmt/format.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

constexpr int32_t log2(int32_t n) {
    return (n > 1) ? 1 + log2(n >> 1) : 0;
}

namespace ls {

SDLGamepad::SDLGamepad(const CallbackInfo& info) : ObjectWrap<SDLGamepad>(info) {
    auto env{ info.Env() };
    auto index{ info[0].As<Number>().Int32Value() };

    this->joystick = SDL_JoystickOpen(index);

    if (!this->joystick) {
        throw Error::New(env, fmt::format("Could not open SDL Joystick at index {}", index));
    }

    auto joystickName = SDL_JoystickName(this->joystick);

    this->name = joystickName ? joystickName : "";

    auto joystickGUID{ SDL_JoystickGetGUID(this->joystick) };
    static char guid[33];

    guid[0] = '\0';
    SDL_JoystickGetGUIDString(joystickGUID, guid, 33);

    this->id = SDL_JoystickInstanceID(this->joystick);
    this->uuid = guid;
    this->physicalButtonCount = SDL_JoystickNumButtons(this->joystick);
    this->hatCount = SDL_JoystickNumHats(this->joystick);
    this->axisCount = SDL_JoystickNumAxes(this->joystick);
    this->hatState.resize(this->hatCount);
//    TODO: not available in 2.0.4..
//    this->product = SDL_JoystickGetProduct(this->joystick);
//    this->productVersion = SDL_JoystickGetProductVersion(this->joystick);
//    this->vendor = SDL_JoystickGetVendor(this->joystick);

    auto value{ SDL_GameControllerMappingForGUID(joystickGUID) };

    if (value) {
        this->gameControllerMapping = value;
    }
}

SDLGamepad::~SDLGamepad() {
    if (this->joystick) {
        fmt::println("Warning: SDLGamepad was not destroyed.");
    }
}

Function SDLGamepad::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SDLGamepad", {
            InstanceValue("type", String::New(env, "gamepad")),
            InstanceAccessor("id", &InputDevice::GetId, nullptr),
            InstanceAccessor("uuid", &InputDevice::GetUUID, nullptr),
            InstanceAccessor("mapping", &InputDevice::GetMapping, nullptr),
            InstanceAccessor("name", &InputDevice::GetName, nullptr),
            InstanceAccessor("buttonCount", &SDLGamepad::GetButtonCount, nullptr),
            InstanceAccessor("axisCount", &SDLGamepad::GetAxisCount, nullptr),
            InstanceAccessor("product", &SDLGamepad::GetProduct, nullptr),
            InstanceAccessor("productVersion", &SDLGamepad::GetProductVersion, nullptr),
            InstanceAccessor("vendor", &SDLGamepad::GetVendor, nullptr),
            InstanceAccessor("gameControllerMapping", &SDLGamepad::GetGameControllerMapping, nullptr),
            InstanceMethod("isButtonDown", &SDLGamepad::IsButtonDown),
            InstanceMethod("getAxisValue", &SDLGamepad::GetAxisValue),
            InstanceMethod("destroy", &SDLGamepad::Destroy),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value SDLGamepad::IsButtonDown(const CallbackInfo& info) {
    bool state;

    if (this->joystick) {
        auto buttonIndex{ info[0].As<Number>().Int32Value() };

        if (buttonIndex > this->physicalButtonCount) {
            buttonIndex -= this->physicalButtonCount;

            auto hatIndex{ buttonIndex / 4 };

            state = SDL_JoystickGetHat(this->joystick, hatIndex) == std::pow(2, buttonIndex + (hatIndex * 4));
        } else {
            state = SDL_JoystickGetButton(this->joystick, buttonIndex) != 0;
        }
    } else {
        state = false;
    }

    return Boolean::New(info.Env(), state);
}

Value SDLGamepad::GetAxisValue(const CallbackInfo& info) {
    return Boolean::New(info.Env(), false);
}

Value SDLGamepad::GetButtonCount(const CallbackInfo& info) {
    return Number::New(info.Env(), this->physicalButtonCount + (this->hatCount * 4));
}

Value SDLGamepad::GetAxisCount(const CallbackInfo& info) {
    return Number::New(info.Env(), this->axisCount);
}

Value SDLGamepad::GetProduct(const CallbackInfo& info) {
    return Number::New(info.Env(), this->product);
}

Value SDLGamepad::GetVendor(const CallbackInfo& info) {
    return Number::New(info.Env(), this->vendor);
}

Value SDLGamepad::GetProductVersion(const CallbackInfo& info) {
    return Number::New(info.Env(), this->productVersion);
}

Value SDLGamepad::GetGameControllerMapping(const CallbackInfo& info) {
    return String::New(info.Env(), this->gameControllerMapping);
}

void SDLGamepad::Destroy(const CallbackInfo& info) {
    this->Destroy();
}

void SDLGamepad::Destroy() {
    if (this->joystick) {
        SDL_JoystickClose(this->joystick);
        this->joystick = nullptr;
    }
}

int32_t SDLGamepad::GetHatButtonIndex(uint8_t hatIndex, uint8_t hatValue) const {
  auto exp = log2(hatValue);

  return (exp >= 0 && exp < 4) ? this->physicalButtonCount + (hatIndex * 4) + exp : -1;
}

void SDLGamepad::SetHatState(uint8_t hatIndex, uint8_t hatValue) {
    if (hatIndex < this->hatCount) {
        this->hatState[hatIndex] = hatValue;
    }
}

uint8_t SDLGamepad::GetHatState(uint8_t hatIndex) const {
    return (hatIndex < this->hatCount) ? this->hatState[hatIndex] : 0;
}

} // namespace ls
