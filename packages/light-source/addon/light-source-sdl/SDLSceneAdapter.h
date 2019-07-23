/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <SceneAdapter.h>
#include <SDL.h>

namespace ls {

class SDLSceneAdapter : public SceneAdapter, public Napi::ObjectWrap<SDLSceneAdapter> {
 public:
    explicit SDLSceneAdapter(const Napi::CallbackInfo& info);
    virtual ~SDLSceneAdapter() = default;

    static Napi::Function Constructor(Napi::Env env);
    Napi::Value Attach(const Napi::CallbackInfo& info);
    Napi::Value Detach(const Napi::CallbackInfo& info);
    Napi::Value Resize(const Napi::CallbackInfo& info);

 private:
    SDL_Window* window{};
};

} // namespace ls
