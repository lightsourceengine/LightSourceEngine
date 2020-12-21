/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/SDLMixerAudioPluginImpl.h>

#include <lse/SDL2.h>
#include <lse/SDL2_mixer.h>
#include <lse/AudioDestination.h>
#include <lse/AudioSource.h>
#include <lse/string-ext.h>
#include <lse/Log.h>
#include <lse/Timer.h>
#include <algorithm>
#include <std17/algorithm>

using Napi::Boolean;
using Napi::Buffer;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::Value;

namespace lse {

SDL_RWops* LoadRW(Napi::Env env, const Napi::Value& value);
void FillVector(
    std::vector<std::string>* destination, const std::function<const char*(int32_t)>& get,
    int32_t len) noexcept;
float ConstrainVolume(float volume) noexcept;
int32_t GetFadeOutMs(const CallbackInfo& info);
int32_t GetFadeInMs(const CallbackInfo& info);
int32_t GetLoops(const CallbackInfo& info);

constexpr auto MIX_MAX_VOLUME_F{ static_cast<float>(MIX_MAX_VOLUME) };

class SDLMixerSampleAudioSourceImpl final : public AudioSourceInterface {
 public:
  explicit SDLMixerSampleAudioSourceImpl(const CallbackInfo& info) {}
  virtual ~SDLMixerSampleAudioSourceImpl() = default;

  void Load(const Napi::CallbackInfo& info) override {
    this->Destroy();

    auto env{ info.Env() };

    Timer t("sample: load");

    this->chunk = SDL2::mixer::Mix_LoadWAV_RW(LoadRW(env, info[0]), SDL_TRUE);

    if (!this->chunk) {
      throw Error::New(env, "Load() failed to load audio data.");
    }
  }

  void Destroy(const Napi::CallbackInfo& info) override {
    this->Destroy();
  }

  Napi::Value GetVolume(const Napi::CallbackInfo& info) override {
    if (this->chunk) {
      return Number::New(info.Env(), ConstrainVolume(
          static_cast<float>(SDL2::mixer::Mix_VolumeChunk(this->chunk, -1)) / MIX_MAX_VOLUME_F));
    }

    return Number::New(info.Env(), 0);
  }

  void SetVolume(const Napi::CallbackInfo& info, const Napi::Value& value) override {
    if (this->chunk) {
      const auto volume{ ConstrainVolume(info[0].As<Number>().FloatValue()) };

      SDL2::mixer::Mix_VolumeChunk(this->chunk, static_cast<int32_t>(volume * MIX_MAX_VOLUME_F));
    }
  }

  Napi::Value HasCapability(const Napi::CallbackInfo& info) override {
    switch (info[0].As<Number>().Int32Value()) {
      case AudioSourceCapabilityFadeIn:
      case AudioSourceCapabilityVolume:
      case AudioSourceCapabilityLoop:
        return Boolean::New(info.Env(), true);
      default:
        return Boolean::New(info.Env(), false);
    }
  }

  void Play(const Napi::CallbackInfo& info) override {
    if (this->chunk) {
      int32_t result;
      auto fadeInMs{ GetFadeInMs(info) };

      if (fadeInMs > 0) {
        result = SDL2::mixer::Mix_FadeInChannelTimed(-1, this->chunk, GetLoops(info) - 1, fadeInMs, -1);
      } else {
        result = SDL2::mixer::Mix_PlayChannelTimed(-1, this->chunk, GetLoops(info) - 1, -1);
      }

      if (result < 0) {
        LOG_ERROR(SDL2::SDL_GetError());
      }
    }
  }

  void Finalize() override {
    delete this;
  }

 private:
  void Destroy() {
    if (this->chunk) {
      SDL2::mixer::Mix_FreeChunk(this->chunk);
      this->chunk = nullptr;
    }
  }

 private:
  Mix_Chunk* chunk{ nullptr };
};

class SDLMixerSampleAudioDestinationImpl final : public AudioDestinationInterface {
 public:
  explicit SDLMixerSampleAudioDestinationImpl(const CallbackInfo& info) {
    FillVector(
        &this->decoders,
        [](int32_t i) { return SDL2::mixer::Mix_GetChunkDecoder(i); },
        SDL2::mixer::Mix_GetNumChunkDecoders());
  }

  virtual ~SDLMixerSampleAudioDestinationImpl() = default;

  Napi::Value CreateAudioSource(const CallbackInfo& info) override {
    return AudioSource::Create<SDLMixerSampleAudioSourceImpl>(info.Env());
  }

  Napi::Value GetVolume(const CallbackInfo& info) override {
    return Number::New(info.Env(),
                       ConstrainVolume(static_cast<float>(SDL2::mixer::Mix_Volume(-1, -1)) / MIX_MAX_VOLUME_F));
  }

  void SetVolume(const CallbackInfo& info, const Napi::Value& value) override {
    auto volume{ ConstrainVolume(info[0].As<Number>().FloatValue()) };

    SDL2::mixer::Mix_Volume(-1, static_cast<int32_t>(volume * MIX_MAX_VOLUME_F));
  }

  void Pause(const CallbackInfo& info) override {
    SDL2::mixer::Mix_Pause(-1);
  }

  void Resume(const CallbackInfo& info) override {
    SDL2::mixer::Mix_Resume(-1);
  }

  void Stop(const CallbackInfo& info) override {
    auto fadeOutMs{ GetFadeOutMs(info) };

    if (fadeOutMs > 0) {
      SDL2::mixer::Mix_FadeOutChannel(-1, fadeOutMs);
    } else {
      SDL2::mixer::Mix_HaltChannel(-1);
    }
  }

  Napi::Value HasCapability(const CallbackInfo& info) override {
    // all capabilities supported
    return Boolean::New(info.Env(), true);
  }

  void Destroy(const CallbackInfo& info) override {
  }

  Value GetDecoders(const CallbackInfo& info) override {
    return Napi::NewStringArray(info.Env(), this->decoders);
  }

  void Finalize() override {
    delete this;
  }

 private:
  std::vector<std::string> decoders{};
};

class SDLMixerStreamAudioSourceImpl final : public AudioSourceInterface {
 public:
  explicit SDLMixerStreamAudioSourceImpl(const CallbackInfo& info) {}
  virtual ~SDLMixerStreamAudioSourceImpl() = default;

  void Load(const CallbackInfo& info) override {
    this->Destroy();

    auto env{ info.Env() };

    Timer t("music: load mus");

    this->music = SDL2::mixer::Mix_LoadMUS_RW(LoadRW(env, info[0]), SDL_TRUE);

    if (!this->music) {
      throw Error::New(env, "Load() failed to load audio data.");
    }
  }

  void Destroy(const CallbackInfo& info) override {
    this->Destroy();
  }

  Napi::Value HasCapability(const CallbackInfo& info) override {
    switch (info[0].As<Number>().Int32Value()) {
      case AudioSourceCapabilityFadeIn:
      case AudioSourceCapabilityLoop:
        return Boolean::New(info.Env(), true);
      default:
        return Boolean::New(info.Env(), false);
    }
  }

  void Play(const CallbackInfo& info) override {
    if (this->music) {
      const auto fadeInMs{ GetFadeInMs(info) };
      int32_t result;

      if (fadeInMs > 0) {
        result = SDL2::mixer::Mix_FadeInMusic(this->music, GetLoops(info) - 1, fadeInMs);
      } else {
        result = SDL2::mixer::Mix_PlayMusic(this->music, GetLoops(info) - 1);
      }

      if (result < 0) {
        LOG_ERROR(SDL2::SDL_GetError());
      }
    }
  }

  Value GetVolume(const CallbackInfo& info) override {
    return Number::New(info.Env(), 0);
  }

  void SetVolume(const CallbackInfo& info, const Value& value) override {
  }

  void Finalize() override {
    delete this;
  }

 private:
  void Destroy() {
    if (this->music) {
      SDL2::mixer::Mix_FreeMusic(this->music);
      this->music = nullptr;
    }
  }

 private:
  Mix_Music* music{ nullptr };
};

class SDLMixerStreamAudioDestinationImpl final : public AudioDestinationInterface {
 public:
  explicit SDLMixerStreamAudioDestinationImpl(const CallbackInfo& info) {
    FillVector(
        &this->decoders,
        [](int32_t i) { return SDL2::mixer::Mix_GetMusicDecoder(i); },
        SDL2::mixer::Mix_GetNumMusicDecoders());
  }

  virtual ~SDLMixerStreamAudioDestinationImpl() = default;

  Napi::Value CreateAudioSource(const CallbackInfo& info) override {
    return AudioSource::Create<SDLMixerStreamAudioSourceImpl>(info.Env());
  }

  Napi::Value GetVolume(const CallbackInfo& info) override {
    return Number::New(info.Env(),
                       ConstrainVolume(static_cast<float>(SDL2::mixer::Mix_VolumeMusic(-1)) / MIX_MAX_VOLUME_F));
  }

  void SetVolume(const CallbackInfo& info, const Napi::Value& value) override {
    auto volume{ ConstrainVolume(info[0].As<Number>().FloatValue()) };

    SDL2::mixer::Mix_VolumeMusic(static_cast<int32_t>(volume * MIX_MAX_VOLUME_F));
  }

  void Pause(const CallbackInfo& info) override {
    SDL2::mixer::Mix_PauseMusic();
  }

  void Resume(const CallbackInfo& info) override {
    SDL2::mixer::Mix_ResumeMusic();
  }

  void Stop(const CallbackInfo& info) override {
    auto fadeOutMs{ GetFadeOutMs(info) };

    if (fadeOutMs > 0) {
      SDL2::mixer::Mix_FadeOutMusic(fadeOutMs);
    } else {
      SDL2::mixer::Mix_HaltMusic();
    }
  }

  Napi::Value HasCapability(const CallbackInfo& info) override {
    // all capabilities supported
    return Boolean::New(info.Env(), true);
  }

  void Destroy(const CallbackInfo& info) override {
  }

  Value GetDecoders(const CallbackInfo& info) override {
    return Napi::NewStringArray(info.Env(), this->decoders);
  }

  void Finalize() override {
    delete this;
  }

 private:
  std::vector<std::string> decoders{};
};

SDLMixerAudioPluginImpl::SDLMixerAudioPluginImpl(const CallbackInfo& info) {
}

void SDLMixerAudioPluginImpl::Attach(const CallbackInfo& info) {
  if (this->isAttached) {
    return;
  }

  auto env{ info.Env() };

  if (SDL2::SDL_WasInit(SDL_INIT_AUDIO) == 0 && SDL2::SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    throw Error::New(env, Format("Failed to init SDL audio. SDL Error: %s", SDL2::SDL_GetError()));
  }

  if (SDL2::mixer::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512) < 0) {
    throw Error::New(env, Format("Cannot open mixer. Error: %s", SDL2::SDL_GetError()));
  }

  // TODO: MIX_INIT_OPUS and MIX_INIT_MID not available in 2.0.0 headers.. ok to patch it in
  SDL2::mixer::Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | /*MIX_INIT_MID*/ 0x00000020
                            | /*MIX_INIT_OPUS*/ 0x00000040);

  FillVector(
      &this->audioDevices,
      [](int32_t i) -> const char* {
        return SDL2::SDL_GetAudioDeviceName(i, 0);
      },
      SDL2::SDL_GetNumAudioDevices(0));

  this->isAttached = true;
}

void SDLMixerAudioPluginImpl::Detach(const CallbackInfo& info) {
  if (!this->isAttached) {
    return;
  }

  SDL2::mixer::Mix_CloseAudio();

  // Documentation recommended way to call Mix_Quit()
  while (SDL2::mixer::Mix_Init(0)) {
    SDL2::mixer::Mix_Quit();
  }

  SDL2::SDL_QuitSubSystem(SDL_INIT_AUDIO);

  this->isAttached = false;
}

Value SDLMixerAudioPluginImpl::IsAttached(const CallbackInfo& info) {
  return Boolean::New(info.Env(), this->isAttached);
}

Value SDLMixerAudioPluginImpl::GetAudioDevices(const CallbackInfo& info) {
  return NewStringArray(info.Env(), this->audioDevices);
}

void SDLMixerAudioPluginImpl::Destroy(const Napi::CallbackInfo& info) {
  this->Detach(info);
}

Value SDLMixerAudioPluginImpl::CreateSampleAudioDestination(const CallbackInfo& info) {
  auto env{ info.Env() };

  if (!this->isAttached) {
    throw Error::New(env, "SDLMixerAudioPlugin is not attached!");
  }

  return AudioDestination::Create<SDLMixerSampleAudioDestinationImpl>(info.Env());
}

Value SDLMixerAudioPluginImpl::CreateStreamAudioDestination(const CallbackInfo& info) {
  auto env{ info.Env() };

  if (!this->isAttached) {
    throw Error::New(env, "SDLMixerAudioPlugin is not attached!");
  }

  return AudioDestination::Create<SDLMixerStreamAudioDestinationImpl>(info.Env());
}

void SDLMixerAudioPluginImpl::Finalize() {
  delete this;
}

SDL_RWops* LoadRW(Napi::Env env, const Napi::Value& value) {
  if (!value.IsBuffer()) {
    throw Error::New(env, "Load() expects a Buffer");
  }

  auto buffer{ value.As<Buffer<uint8_t>>() };
  auto data{ static_cast<void*>(buffer.Data()) };
  auto len{ static_cast<int32_t>(buffer.Length()) };
  SDL_RWops* src{ nullptr };

  if (data && len > 0) {
    src = SDL2::SDL_RWFromMem(data, len);
  }

  if (!src) {
    throw Error::New(env, "Load() failed to read audio file from Buffer.");
  }

  return src;
}

void FillVector(
    std::vector<std::string>* destination, const std::function<const char*(int32_t)>& get,
    const int32_t len) noexcept {
  destination->clear();
  destination->reserve(len);

  for (int32_t i{ 0 }; i < len; i++) {
    const auto str{ get(i) };

    destination->emplace_back(str ? str : "");
  }
}

float ConstrainVolume(const float volume) noexcept {
  return std17::clamp(volume, 0.f, 1.f);
}

int32_t GetPropertyAsInt(const CallbackInfo& info, const char* property, const int32_t defaultValue) {
  if (info.Length() > 0 && info[0].IsObject()) {
    return Napi::ObjectGetNumberOrDefault(info[0].As<Object>(), property, 0);
  }

  return defaultValue;
}

int32_t GetFadeOutMs(const CallbackInfo& info) {
  return GetPropertyAsInt(info, "fadeOutMs", 0);
}

int32_t GetFadeInMs(const CallbackInfo& info) {
  return GetPropertyAsInt(info, "fadeInMs", 0);
}

int32_t GetLoops(const CallbackInfo& info) {
  return GetPropertyAsInt(info, "loops", 1);
}

} // namespace lse
