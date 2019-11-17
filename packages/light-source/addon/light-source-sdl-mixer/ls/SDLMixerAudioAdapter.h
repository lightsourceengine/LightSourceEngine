/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/BaseAudioAdapter.h>

namespace ls {

class SDLMixerAudioAdapter : public Napi::SafeObjectWrap<SDLMixerAudioAdapter>, public BaseAudioAdapter {
 public:
    explicit SDLMixerAudioAdapter(const Napi::CallbackInfo& info);
    virtual ~SDLMixerAudioAdapter() = default;

    static Napi::Function GetClass(Napi::Env env);

 private: // javascript bindings
    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;

    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
    Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

    friend Napi::SafeObjectWrap<SDLMixerAudioAdapter>;
    friend BaseAudioAdapter;
};

} // namespace ls
