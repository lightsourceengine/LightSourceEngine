/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <BaseAudioAdapter.h>
#include <SDL.h>

namespace ls {

class SDLAudioAdapter : public Napi::ObjectWrap<SDLAudioAdapter>, public BaseAudioAdapter {
 public:
    explicit SDLAudioAdapter(const Napi::CallbackInfo& info);
    virtual ~SDLAudioAdapter() = default;

    static Napi::Function Constructor(Napi::Env env);

    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;

 private:
    int32_t deviceId{-1};
};

} // namespace ls
