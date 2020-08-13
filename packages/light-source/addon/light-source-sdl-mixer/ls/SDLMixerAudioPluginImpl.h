/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/AudioPlugin.h>

namespace ls {

class SDLMixerAudioPluginImpl final : public AudioPluginInterface {
 public:
    SDLMixerAudioPluginImpl(const Napi::CallbackInfo& info);
    virtual ~SDLMixerAudioPluginImpl() = default;

    void Attach(const Napi::CallbackInfo& info) override;
    void Detach(const Napi::CallbackInfo& info) override;
    void Destroy(const Napi::CallbackInfo& info) override;
    Napi::Value IsAttached(const Napi::CallbackInfo& info) override;
    Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) override;
    Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
    Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

    void Finalize() override;

 private:
    bool isAttached{false};
    std::vector<std::string> audioDevices{};
};

} // namespace ls
