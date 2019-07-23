/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLSceneAdapter.h"

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::ObjectWrap;
using Napi::Value;

namespace ls {

SDLSceneAdapter::SDLSceneAdapter(const CallbackInfo& info) : ObjectWrap<SDLSceneAdapter>(info) {
}

Function SDLSceneAdapter::Constructor(Napi::Env env) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "SDLSceneAdapter", {
            InstanceMethod("attach", &SDLSceneAdapter::Attach),
            InstanceMethod("detach", &SDLSceneAdapter::Detach),
            InstanceMethod("resize", &SDLSceneAdapter::Resize),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.Value();
}

Value SDLSceneAdapter::Attach(const CallbackInfo& info) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    this->window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);

    return info.Env().Undefined();
}

Value SDLSceneAdapter::Detach(const CallbackInfo& info) {
    return info.Env().Undefined();
}

Value SDLSceneAdapter::Resize(const CallbackInfo& info) {
    return info.Env().Undefined();
}

} // namespace ls
