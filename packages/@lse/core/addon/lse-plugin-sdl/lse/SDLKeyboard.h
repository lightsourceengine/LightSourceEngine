/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/InputDevice.h>

namespace lse {

class SDLKeyboard final : public Napi::SafeObjectWrap<SDLKeyboard>, public InputDevice {
 public:
    SDLKeyboard(const Napi::CallbackInfo& info);
    ~SDLKeyboard() override = default;

 public:
    static Napi::Function GetClass(Napi::Env env);

    Napi::Value IsButtonDown(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
};

} // namespace lse
