/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLKeyboard.h"
#include <SDL.h>

using Napi::Boolean;
using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::ObjectWrap;
using Napi::String;
using Napi::Value;

namespace ls {

SDLKeyboard::SDLKeyboard(const CallbackInfo& info) : ObjectWrap<SDLKeyboard>(info) {
    this->id = 0;
    this->uuid = "keyboard";
    this->name = "System Keyboard";
}

Function SDLKeyboard::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SDLKeyboard", {
            InstanceValue("type", String::New(env, "keyboard"), napi_enumerable),
            InstanceAccessor("id", &InputDevice::GetId, nullptr, napi_enumerable),
            InstanceAccessor("uuid", &InputDevice::GetUUID, nullptr, napi_enumerable),
            InstanceAccessor("mapping", &InputDevice::GetMapping, nullptr, napi_enumerable),
            InstanceAccessor("name", &InputDevice::GetName, nullptr, napi_enumerable),
            InstanceMethod("isKeyDown", &SDLKeyboard::IsKeyDown),
            InstanceMethod("destroy", &SDLKeyboard::Destroy),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value SDLKeyboard::IsKeyDown(const CallbackInfo& info) {
    bool isDown;

    if (info[0].IsNumber()) {
        auto len{0};
        auto keyIndex{info[0].As<Number>().Int32Value()};
        auto state = SDL_GetKeyboardState(&len);

        isDown = (keyIndex > 0 && keyIndex < len && state && state[keyIndex] != 0);
    } else {
        isDown = false;
    }

    return Boolean::New(info.Env(), isDown);
}

void SDLKeyboard::Destroy(const CallbackInfo& info) {
}

} // namespace ls
