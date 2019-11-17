/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <SDL.h>
#include <vector>
#include "InputDevice.h"

namespace ls {

class SDLGamepad : public InputDevice, public Napi::SafeObjectWrap<SDLGamepad> {
 public:
    explicit SDLGamepad(const Napi::CallbackInfo& info);
    virtual ~SDLGamepad();

    void SetHatState(uint8_t hatIndex, uint8_t hatValue);
    uint8_t GetHatState(uint8_t hatIndex) const;
    void Destroy();

 public:
    static SDLGamepad* New(Napi::Env env, int32_t index);
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Constructor(const Napi::CallbackInfo& info) override;
    Napi::Value IsButtonDown(const Napi::CallbackInfo& info);
    Napi::Value GetAxisValue(const Napi::CallbackInfo& info);
    Napi::Value GetHatValue(const Napi::CallbackInfo& info);
    Napi::Value GetButtonCount(const Napi::CallbackInfo& info);
    Napi::Value GetHatCount(const Napi::CallbackInfo& info);
    Napi::Value GetAxisCount(const Napi::CallbackInfo& info);
    Napi::Value GetProduct(const Napi::CallbackInfo& info);
    Napi::Value GetVendor(const Napi::CallbackInfo& info);
    Napi::Value GetProductVersion(const Napi::CallbackInfo& info);
    Napi::Value GetGameControllerMapping(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);

 private:
    SDL_Joystick* joystick{};
    int32_t buttonCount{0};
    int32_t hatCount{0};
    int32_t axisCount{0};
    int32_t product{};
    int32_t productVersion{};
    int32_t vendor{};
    std::vector<uint8_t> hatState;
    std::string gameControllerMapping;

    friend Napi::SafeObjectWrap<SDLGamepad>;
};

} // namespace ls