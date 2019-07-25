/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <SDL.h>
#include <vector>
#include "InputDevice.h"

namespace ls {

class SDLGamepad : public InputDevice, public Napi::ObjectWrap<SDLGamepad> {
 public:
    explicit SDLGamepad(const Napi::CallbackInfo& info);
    virtual ~SDLGamepad();

    static Napi::Function Constructor(Napi::Env env);

    Napi::Value IsButtonDown(const Napi::CallbackInfo& info);
    Napi::Value GetAxisValue(const Napi::CallbackInfo& info);
    Napi::Value GetButtonCount(const Napi::CallbackInfo& info);
    Napi::Value GetAxisCount(const Napi::CallbackInfo& info);
    Napi::Value GetProduct(const Napi::CallbackInfo& info);
    Napi::Value GetVendor(const Napi::CallbackInfo& info);
    Napi::Value GetProductVersion(const Napi::CallbackInfo& info);
    Napi::Value GetPlayerIndex(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);

    int32_t GetHatButtonIndex(uint8_t hatIndex, uint8_t hatValue) const;
    void SetHatState(uint8_t hatIndex, uint8_t hatValue);
    uint8_t GetHatState(uint8_t hatIndex) const;

    void Destroy();

 private:
    SDL_Joystick* joystick{};
    int32_t physicalButtonCount{0};
    int32_t hatCount{0};
    int32_t axisCount{0};
    int32_t product{};
    int32_t productVersion{};
    int32_t vendor{};
    int32_t playerIndex{-1};

    std::vector<uint8_t> hatState;
};

} // namespace ls
