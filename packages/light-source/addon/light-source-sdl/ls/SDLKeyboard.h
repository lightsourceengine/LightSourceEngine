/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include "InputDevice.h"

namespace ls {

class SDLKeyboard : public InputDevice, public Napi::SafeObjectWrap<SDLKeyboard> {
 public:
    explicit SDLKeyboard(const Napi::CallbackInfo& info);
    virtual ~SDLKeyboard() = default;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    Napi::Value IsButtonDown(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);

    friend Napi::SafeObjectWrap<SDLKeyboard>;
};

} // namespace ls
