/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/SDL2.h>
#include <vector>
#include <lse/InputDevice.h>

namespace lse {

class SDLGamepad final : public Napi::SafeObjectWrap<SDLGamepad>, public InputDevice {
 public:
  SDLGamepad(const Napi::CallbackInfo& info);
  ~SDLGamepad() override;

  static SDLGamepad* New(Napi::Env env, int32_t index);
  static Napi::Function GetClass(Napi::Env env);

  void Constructor(const Napi::CallbackInfo& info) override;
  Napi::Value IsKeyDown(const Napi::CallbackInfo& info);
  Napi::Value GetAnalogValue(const Napi::CallbackInfo& info);
  Napi::Value GetButtonState(const Napi::CallbackInfo& info);
  Napi::Value GetAxisState(const Napi::CallbackInfo& info);
  Napi::Value GetHatState(const Napi::CallbackInfo& info);
  Napi::Value GetButtonCount(const Napi::CallbackInfo& info);
  Napi::Value GetHatCount(const Napi::CallbackInfo& info);
  Napi::Value GetAxisCount(const Napi::CallbackInfo& info);
  Napi::Value GetProduct(const Napi::CallbackInfo& info);
  Napi::Value GetVendor(const Napi::CallbackInfo& info);
  Napi::Value GetProductVersion(const Napi::CallbackInfo& info);
  Napi::Value GetGameControllerMapping(const Napi::CallbackInfo& info);
  void Destroy(const Napi::CallbackInfo& info);

  void Destroy();

 private:
  SDL_Joystick* joystick{};
  SDL_GameController* gameController{};
  int32_t buttonCount{ 0 };
  int32_t hatCount{ 0 };
  int32_t axisCount{ 0 };
  int32_t product{};
  int32_t productVersion{};
  int32_t vendor{};
  std::string gameControllerMapping;
};

} // namespace lse
