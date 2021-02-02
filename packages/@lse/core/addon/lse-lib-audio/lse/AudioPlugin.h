/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace lse {

class AudioPluginInterface {
 public:
  virtual void Attach(const Napi::CallbackInfo& info) = 0;
  virtual void Detach(const Napi::CallbackInfo& info) = 0;
  virtual void Destroy(const Napi::CallbackInfo& info) = 0;
  virtual Napi::Value IsAttached(const Napi::CallbackInfo& info) = 0;
  virtual Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) = 0;
  virtual Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) = 0;
  virtual Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) = 0;

  virtual void Finalize() = 0;
};

using AudioPluginInterfaceFactory = AudioPluginInterface* (*)(const Napi::CallbackInfo& info);

class AudioPlugin : public Napi::SafeObjectWrap<AudioPlugin>, public AudioPluginInterface {
 public:
  AudioPlugin(const Napi::CallbackInfo& info);
  ~AudioPlugin() override;

 public:
  void Constructor(const Napi::CallbackInfo& info) override;
  void Attach(const Napi::CallbackInfo& info) override;
  void Detach(const Napi::CallbackInfo& info) override;
  void Destroy(const Napi::CallbackInfo& info) override;
  Napi::Value IsAttached(const Napi::CallbackInfo& info) override;
  Napi::Value GetAudioDevices(const Napi::CallbackInfo& info) override;
  Napi::Value CreateSampleAudioDestination(const Napi::CallbackInfo& info) override;
  Napi::Value CreateStreamAudioDestination(const Napi::CallbackInfo& info) override;

  void Finalize() override;

  static Napi::Function GetClass(Napi::Env env);

 private:
  AudioPluginInterface* impl{};
};

template<typename T>
Napi::Value AudioPluginInit(Napi::Env env) {
  const AudioPluginInterfaceFactory factory{
      [](const Napi::CallbackInfo& info) -> AudioPluginInterface* {
        return new T(info);
      }
  };

  Napi::EscapableHandleScope scope(env);
  auto external{ Napi::External<void>::New(env, reinterpret_cast<void*>(factory)) };

  return scope.Escape(AudioPlugin::GetClass(env).New({ external }));
}

} // namespace lse
