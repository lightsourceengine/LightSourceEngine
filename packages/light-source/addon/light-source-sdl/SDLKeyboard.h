/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include "InputDevice.h"

namespace ls {

class SDLKeyboard : public InputDevice, public Napi::ObjectWrap<SDLKeyboard> {
 public:
    explicit SDLKeyboard(const Napi::CallbackInfo& info);
    virtual ~SDLKeyboard() = default;

    static Napi::Function Constructor(Napi::Env env);

    Napi::Value IsKeyDown(const Napi::CallbackInfo& info);
    void Destroy(const Napi::CallbackInfo& info);
};

} // namespace ls
