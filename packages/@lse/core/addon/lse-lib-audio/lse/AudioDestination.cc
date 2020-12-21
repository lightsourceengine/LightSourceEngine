/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "AudioDestination.h"

#define CHECK_IMPL(impl) if (!(impl)) { throw Napi::Error::New(info.Env(), "AudioDestination is not initialized!"); }

namespace lse {

AudioDestination::AudioDestination(const Napi::CallbackInfo& info) : Napi::SafeObjectWrap<AudioDestination>(info) {
}

AudioDestination::~AudioDestination() {
  if (this->impl) {
    this->impl->Finalize();
  }
}

void AudioDestination::Constructor(const Napi::CallbackInfo& info) {
  this->impl = Napi::ConstructorWithExternalFactory<AudioDestinationInterface, AudioDestinationInterfaceFactory>(
      info, "AudioDestination");
}

Napi::Function AudioDestination::GetClass(Napi::Env env) {
  static Napi::FunctionReference constructor;

  if (constructor.IsEmpty()) {
    Napi::HandleScope scope(env);

    constructor = AudioDestination::DefineClass(env, "AudioDestination", true, {
        AudioDestination::InstanceMethod("createAudioSource", &AudioDestination::CreateAudioSource),
        AudioDestination::InstanceMethod("resume", &AudioDestination::Resume),
        AudioDestination::InstanceMethod("pause", &AudioDestination::Pause),
        AudioDestination::InstanceMethod("stop", &AudioDestination::Stop),
        AudioDestination::InstanceMethod("destroy", &AudioDestination::Destroy),
        AudioDestination::InstanceMethod("hasCapability", &AudioDestination::HasCapability),
        AudioDestination::InstanceAccessor("volume", &AudioDestination::GetVolume, &AudioDestination::SetVolume),
        AudioDestination::InstanceAccessor("decoders", &AudioDestination::GetDecoders),
    });
  }

  return constructor.Value();
}

void AudioDestination::Destroy(const Napi::CallbackInfo& info) {
  if (this->impl) {
    this->impl->Destroy(info);
    this->impl->Finalize();
    this->impl = nullptr;
  }
}

Napi::Value AudioDestination::GetDecoders(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->GetDecoders(info);
}

Napi::Value AudioDestination::CreateAudioSource(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->CreateAudioSource(info);
}

void AudioDestination::Resume(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->Resume(info);
}

void AudioDestination::Pause(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->Pause(info);
}

void AudioDestination::Stop(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  this->impl->Stop(info);
}

Napi::Value AudioDestination::GetVolume(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->GetVolume(info);
}

void AudioDestination::SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) {
  CHECK_IMPL(this->impl);
  this->impl->SetVolume(info, value);
}

Napi::Value AudioDestination::HasCapability(const Napi::CallbackInfo& info) {
  CHECK_IMPL(this->impl);
  return this->impl->HasCapability(info);
}

void AudioDestination::Finalize() {
  throw std::runtime_error("Not implemented");
}

} // namespace lse

#undef CHECK_IMPL
