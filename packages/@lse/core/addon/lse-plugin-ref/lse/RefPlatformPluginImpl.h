/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/PlatformPlugin.h>

namespace lse {

class RefPlatformPluginImpl final : public PlatformPluginInterface {
 public:
  explicit RefPlatformPluginImpl(const Napi::CallbackInfo& info);
  virtual ~RefPlatformPluginImpl() = default;

 private:
  Napi::Value GetKeyboard(const Napi::CallbackInfo& info) override;
  Napi::Value GetGamepads(const Napi::CallbackInfo& info) override;
  Napi::Value GetCapabilities(const Napi::CallbackInfo& info) override;
  Napi::Value ProcessEvents(const Napi::CallbackInfo& info) override;
  void Attach(const Napi::CallbackInfo& info) override;
  void Detach(const Napi::CallbackInfo& info) override;
  void Destroy(const Napi::CallbackInfo& info) override;
  void SetCallback(const Napi::CallbackInfo& info) override;
  void ResetCallbacks(const Napi::CallbackInfo& info) override;
  Napi::Value CreateGraphicsContext(const Napi::CallbackInfo& info) override;
  Napi::Value LoadGameControllerMappings(const Napi::CallbackInfo& info) override;

  void Finalize() override;

 private:
  Napi::ObjectReference capabilitiesRef;
};

} // namespace lse
