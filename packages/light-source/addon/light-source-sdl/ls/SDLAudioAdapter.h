/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/BaseAudioAdapter.h>
#include <SDL.h>

namespace ls {

class SDLAudioAdapter : public Napi::SafeObjectWrap<SDLAudioAdapter>, public BaseAudioAdapter {
 public:
    explicit SDLAudioAdapter(const Napi::CallbackInfo& info);
    virtual ~SDLAudioAdapter() = default;

 public:
    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;

 private:
    int32_t deviceId{-1};

    friend Napi::SafeObjectWrap<SDLAudioAdapter>;
    friend BaseAudioAdapter;
};

} // namespace ls
