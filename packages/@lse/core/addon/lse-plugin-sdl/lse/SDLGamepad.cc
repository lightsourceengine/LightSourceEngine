/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SDLGamepad.h>

#include <lse/Log.h>
#include <lse/string-ext.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::SafeObjectWrap;
using Napi::String;
using Napi::Value;

constexpr int32_t log2(int32_t n) {
  return (n > 1) ? 1 + log2(n >> 1) : 0;
}

namespace lse {

SDLGamepad::SDLGamepad(const CallbackInfo& info) : SafeObjectWrap<SDLGamepad>(info) {
}

SDLGamepad::~SDLGamepad() {
  if (this->joystick) {
    LOG_WARN("SDLGamepad was not destroyed.");
  }
}

void SDLGamepad::Constructor(const Napi::CallbackInfo& info) {
  auto env{ info.Env() };
  auto index{ info[0].As<Number>().Int32Value() };

  this->joystick = SDL2::SDL_JoystickOpen(index);

  if (!this->joystick) {
    throw Error::New(env, Format("Could not open SDL Joystick at index %i", index));
  }

  auto joystickName = SDL2::SDL_JoystickName(this->joystick);

  this->name = joystickName ? joystickName : "";

  auto joystickGUID{ SDL2::SDL_JoystickGetGUID(this->joystick) };
  static char guid[33];

  guid[0] = '\0';
  SDL2::SDL_JoystickGetGUIDString(joystickGUID, guid, 33);

  this->id = SDL2::SDL_JoystickInstanceID(this->joystick);
  this->uuid = guid;
  this->buttonCount = SDL2::SDL_JoystickNumButtons(this->joystick);
  this->hatCount = SDL2::SDL_JoystickNumHats(this->joystick);
  this->axisCount = SDL2::SDL_JoystickNumAxes(this->joystick);
  this->hatState.resize(this->hatCount);
// TODO: not available in 2.0.4..
//    this->product = SDL_JoystickGetProduct(this->joystick);
//    this->productVersion = SDL_JoystickGetProductVersion(this->joystick);
//    this->vendor = SDL_JoystickGetVendor(this->joystick);
}

SDLGamepad* SDLGamepad::New(Napi::Env env, int32_t index) {
  HandleScope scope(env);

  auto jsObject{ GetClass(env).New({ Number::New(env, index) }) };

  return SDLGamepad::Cast(jsObject);
}

Function SDLGamepad::GetClass(Napi::Env env) {
  static FunctionReference constructor;

  if (constructor.IsEmpty()) {
    HandleScope scope(env);

    constructor = DefineClass(env, "SDLGamepad", true, {
        InstanceValue("type", String::New(env, "gamepad")),
        InstanceAccessor("id", &InputDevice::GetId, nullptr),
        InstanceAccessor("uuid", &InputDevice::GetUUID, nullptr),
        InstanceAccessor("name", &InputDevice::GetName, nullptr),
        InstanceAccessor("buttonCount", &SDLGamepad::GetButtonCount, nullptr),
        InstanceAccessor("hatCount", &SDLGamepad::GetHatCount, nullptr),
        InstanceAccessor("axisCount", &SDLGamepad::GetAxisCount, nullptr),
//            InstanceAccessor("product", &SDLGamepad::GetProduct, nullptr),
//            InstanceAccessor("productVersion", &SDLGamepad::GetProductVersion, nullptr),
//            InstanceAccessor("vendor", &SDLGamepad::GetVendor, nullptr),
        InstanceMethod("isButtonDown", &SDLGamepad::IsButtonDown),
        InstanceMethod("getHatValue", &SDLGamepad::GetHatValue),
        InstanceMethod("getAxisValue", &SDLGamepad::GetAxisValue),
        InstanceMethod("destroy", &SDLGamepad::Destroy),
        InstanceMethod("$getGameControllerMapping", &SDLGamepad::GetGameControllerMapping),
    });
  }

  return constructor.Value();
}

Value SDLGamepad::IsButtonDown(const CallbackInfo& info) {
  bool state;

  if (this->joystick) {
    state = SDL2::SDL_JoystickGetButton(this->joystick, info[0].As<Number>().Int32Value()) != 0;
  } else {
    state = false;
  }

  return Boolean::New(info.Env(), state);
}

Value SDLGamepad::GetHatValue(const CallbackInfo& info) {
  int32_t value;

  if (this->joystick) {
    value = this->GetHatState(info[0].As<Number>().Int32Value());
  } else {
    value = 0;
  }

  return Number::New(info.Env(), value);
}

Value SDLGamepad::GetAxisValue(const CallbackInfo& info) {
  return Boolean::New(info.Env(), false);
}

Value SDLGamepad::GetButtonCount(const CallbackInfo& info) {
  return Number::New(info.Env(), this->buttonCount);
}

Value SDLGamepad::GetAxisCount(const CallbackInfo& info) {
  return Number::New(info.Env(), this->axisCount);
}

Value SDLGamepad::GetHatCount(const CallbackInfo& info) {
  return Number::New(info.Env(), this->hatCount);
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
  auto value{ SDL2::SDL_GameControllerMappingForGUID(
      SDL2::SDL_JoystickGetGUID(this->joystick)) };

  if (value) {
    return String::New(info.Env(), value);
  }

  return info.Env().Null();
}

void SDLGamepad::Destroy(const CallbackInfo& info) {
  this->Destroy();
}

void SDLGamepad::Destroy() {
  if (this->joystick) {
    SDL2::SDL_JoystickClose(this->joystick);
    this->joystick = nullptr;
  }
}

void SDLGamepad::SetHatState(uint8_t hatIndex, uint8_t hatValue) {
  if (hatIndex < this->hatCount) {
    this->hatState[hatIndex] = hatValue;
  }
}

uint8_t SDLGamepad::GetHatState(uint8_t hatIndex) const {
  return (hatIndex < this->hatCount) ? this->hatState[hatIndex] : 0;
}

} // namespace lse
