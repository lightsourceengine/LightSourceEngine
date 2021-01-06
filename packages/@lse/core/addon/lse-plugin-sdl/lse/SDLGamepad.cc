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

  if (SDL2::SDL_IsGameController(index) == SDL_TRUE) {
    this->gameController = SDL2::SDL_GameControllerOpen(index);
    this->joystick = SDL2::SDL_GameControllerGetJoystick(this->gameController);
  } else {
    this->gameController = nullptr;
    this->joystick = SDL2::SDL_JoystickOpen(index);

    if (!this->joystick) {
      throw Error::New(env, Format("Could not open SDL Joystick at index %i", index));
    }
  }

  const char* joystickName;

  if (this->gameController) {
    joystickName = SDL2::SDL_GameControllerName(this->gameController);
  } else {
    joystickName = SDL2::SDL_JoystickName(this->joystick);
  }

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
        InstanceMethod("getButtonState", &SDLGamepad::GetButtonState),
        InstanceMethod("getAxisState", &SDLGamepad::GetAxisState),
        InstanceMethod("getHatState", &SDLGamepad::GetHatState),
        InstanceMethod("isKeyDown", &SDLGamepad::IsKeyDown),
        InstanceMethod("getAnalogValue", &SDLGamepad::GetAnalogValue),
        InstanceMethod("destroy", &SDLGamepad::Destroy),
        InstanceMethod("$getGameControllerMapping", &SDLGamepad::GetGameControllerMapping),
    });
  }

  return constructor.Value();
}

Value SDLGamepad::IsKeyDown(const Napi::CallbackInfo& info) {
  bool state;

  if (this->gameController) {
    int32_t rawButton{ Napi::CastNumberOrDefault(info[0], -1) };
    SDL_GameControllerButton button;

    if (rawButton >= 0 && rawButton < SDL_CONTROLLER_BUTTON_MAX) {
      button = static_cast<SDL_GameControllerButton>(rawButton);
    } else {
      button = SDL_CONTROLLER_BUTTON_INVALID;
    }

    state = SDL2::SDL_GameControllerGetButton(this->gameController, button) != 0;
  } else {
    state = false;
  }

  return Boolean::New(info.Env(), state);
}

Value SDLGamepad::GetAnalogValue(const Napi::CallbackInfo& info) {
  float value;

  if (this->gameController) {
    int32_t rawAxis{ Napi::CastNumberOrDefault(info[0], -1) };
    SDL_GameControllerAxis axis;

    if (rawAxis >= 0 && rawAxis < SDL_CONTROLLER_AXIS_MAX) {
      axis = static_cast<SDL_GameControllerAxis>(rawAxis);
    } else {
      axis = SDL_CONTROLLER_AXIS_INVALID;
    }

    auto axisValue{ SDL2::SDL_GameControllerGetAxis(this->gameController, axis) };

    if (axisValue < 0) {
      value = static_cast<float>(axisValue) / SDL_JOYSTICK_AXIS_MIN_F;
    } else {
      value = static_cast<float>(axisValue) / SDL_JOYSTICK_AXIS_MAX_F;
    }
  } else {
    value = 0.f;
  }

  return Number::New(info.Env(), value);
}

Value SDLGamepad::GetButtonState(const Napi::CallbackInfo& info) {
  bool state;

  if (this->joystick) {
    int32_t button = Napi::CastNumberOrDefault(info[0], -1);

    state = SDL2::SDL_JoystickGetButton(this->joystick, button) != 0;
  } else {
    state = false;
  }

  return Boolean::New(info.Env(), state);
}

Value SDLGamepad::GetHatState(const CallbackInfo& info) {
  int32_t value;

  if (this->joystick) {
    int32_t hat = Napi::CastNumberOrDefault(info[0], -1);

    value = SDL2::SDL_JoystickGetHat(this->joystick, hat);
  } else {
    value = SDL_HAT_CENTERED;
  }

  return Number::New(info.Env(), value);
}

Value SDLGamepad::GetAxisState(const CallbackInfo& info) {
  float value;

  if (this->joystick) {
    int32_t axis{ Napi::CastNumberOrDefault(info[0], -1) };
    auto axisValue{ SDL2::SDL_JoystickGetAxis(this->joystick, axis) };

    if (axisValue < 0) {
      value = static_cast<float>(axisValue) / SDL_JOYSTICK_AXIS_MIN_F;
    } else {
      value = static_cast<float>(axisValue) / SDL_JOYSTICK_AXIS_MAX_F;
    }
  } else {
    value = 0.f;
  }

  return Number::New(info.Env(), value);
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
  if (this->gameController) {
    SDL2::SDL_GameControllerClose(this->gameController);
    this->gameController = nullptr;
    this->joystick = nullptr;
  } else if (this->joystick) {
    SDL2::SDL_JoystickClose(this->joystick);
    this->joystick = nullptr;
  }
}

} // namespace lse
