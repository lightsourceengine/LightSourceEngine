/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <lse/AudioPlugin.h>

namespace lse {

class RefAudioPluginImpl final : public AudioPluginInterface {
 public:
  RefAudioPluginImpl(const Napi::CallbackInfo& info);
  virtual ~RefAudioPluginImpl() = default;

  void Attach(const Napi::CallbackInfo& info) override;
  void Detach(const Napi::CallbackInfo& info) override;
  void Destroy(const Napi::CallbackInfo& info) override;
  Napi::Value IsAttached(const Napi::CallbackInfo& info) override;
  Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) override;
  Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
  Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

  void Finalize() override;

 private:
  bool isAttached{ false };
  std::vector<std::string> audioDevices{};
};

} // namespace lse
