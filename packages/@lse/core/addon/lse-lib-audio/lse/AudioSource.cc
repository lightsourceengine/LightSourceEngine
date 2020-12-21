/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "AudioSource.h"
#include <lse/Log.h>

#define CHECK_IMPL(impl) if (!(impl)) { throw Napi::Error::New(info.Env(), "AudioSource is not initialized!"); }

namespace lse {

AudioSource::AudioSource(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<AudioSource>(info) {
}

AudioSource::~AudioSource() {
  if (this->impl) {
    LOG_WARN("AudioSource was not destroyed.");
  }
}

void AudioSource::Constructor(const Napi::CallbackInfo& info) {
  this->impl = Napi::ConstructorWithExternalFactory<AudioSourceInterface, AudioSourceInterfaceFactory>(
      info, "AudioSource");
}

Napi::Function AudioSource::GetClass(Napi::Env env) {
  static Napi::FunctionReference constructor;

  if (constructor.IsEmpty()) {
    Napi::HandleScope scope(env);

    constructor = AudioSource::DefineClass(env, "AudioSource", true, {
        AudioSource::InstanceMethod("load", &AudioSource::Load),
        AudioSource::InstanceMethod("destroy", &AudioSource::Destroy),
        AudioSource::InstanceMethod("play", &AudioSource::Play),
        AudioSource::InstanceMethod("hasCapability", &AudioSource::HasCapability),
        AudioSource::InstanceAccessor("volume", &AudioSource::GetVolume, &AudioSource::SetVolume),
    });
  }

  return constructor.Value();
}

void AudioSource::Load(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->Load(info);
}

void AudioSource::Destroy(const Napi::CallbackInfo& info) {
  if (this->impl) {
    this->impl->Destroy(info);
    this->impl->Finalize();
    this->impl = nullptr;
  }
}

void AudioSource::Play(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->Play(info);
}

Napi::Value AudioSource::GetVolume(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->GetVolume(info);
}

void AudioSource::SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) {
  CHECK_IMPL(this->impl);
  this->impl->SetVolume(info, value);
}

Napi::Value AudioSource::HasCapability(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->HasCapability(info);
}

void AudioSource::Finalize() {
  throw std::runtime_error("Not implemented");
}

} // namespace lse

#undef CHECK_IMPL