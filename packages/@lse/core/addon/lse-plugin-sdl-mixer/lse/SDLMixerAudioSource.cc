/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLMixerAudioSource.h"

#include <stdexcept>
#include <lse/Log.h>
#include <lse/AudioUtil.h>

namespace lse {

void SDLMixerSampleSource::Load(const char* filename) {
  if (this->chunk) {
    throw std::runtime_error("source already loaded");
  }

  this->Load(SDL2::SDL_RWFromFile(filename, "rb"));
}

void SDLMixerSampleSource::Load(void* buffer, size_t bufferSize) {
  if (this->chunk) {
    throw std::runtime_error("source already loaded");
  }

  this->Load(SDL2::SDL_RWFromMem(buffer, bufferSize));
}

void SDLMixerSampleSource::Load(SDL_RWops* rw) {
  if (!rw) {
    throw std::runtime_error(SDL2::SDL_GetError());
  }

  this->chunk = SDL2::mixer::Mix_LoadWAV_RW(rw, SDL_TRUE);

  if (!this->chunk) {
    throw std::runtime_error(SDL2::SDL_GetError());
  }
}

void SDLMixerSampleSource::Unload() {
  if (this->chunk) {
    SDL2::mixer::Mix_FreeChunk(this->chunk);
    this->chunk = nullptr;
  }
}

void SDLMixerSampleSource::Destroy() {
  this->Unload();
}

void SDLMixerSampleSource::Play(int32_t fadeInMs, int32_t loops) {
  if (!this->chunk) {
    return;
  }

  int32_t result;
  if (fadeInMs > 0) {
    result = SDL2::mixer::Mix_FadeInChannelTimed(ALL_CHANNELS, this->chunk, loops - 1, fadeInMs, -1);
  } else {
    result = SDL2::mixer::Mix_PlayChannelTimed(ALL_CHANNELS, this->chunk, loops - 1, -1);
  }

  if (result < 0) {
    LOG_ERROR(SDL2::SDL_GetError());
  }
}

float SDLMixerSampleSource::GetVolume() {
  if (!this->chunk) {
    return 0;
  }

  return ConstrainVolume(static_cast<float>(SDL2::mixer::Mix_VolumeChunk(this->chunk, -1)) / MIX_MAX_VOLUME_F);
}

void SDLMixerSampleSource::SetVolume(float volume) {
  if (!this->chunk) {
    return;
  }

  SDL2::mixer::Mix_VolumeChunk(this->chunk, static_cast<int32_t>(ConstrainVolume(volume) * MIX_MAX_VOLUME_F));
}

bool SDLMixerSampleSource::HasCapability(AudioSourceCapability capability) {
  switch (capability) {
    case AudioSourceCapabilityFadeIn:
    case AudioSourceCapabilityVolume:
    case AudioSourceCapabilityLoop:
      return true;
    default:
      return false;
  }
}

void SDLMixerStreamSource::Load(const char* filename) {
  if (this->music) {
    throw std::runtime_error("source already loaded");
  }

  this->Load(SDL2::SDL_RWFromFile(filename, "rb"));
}

void SDLMixerStreamSource::Load(void* buffer, size_t bufferSize) {
  if (this->music) {
    throw std::runtime_error("source already loaded");
  }

  this->Load(SDL2::SDL_RWFromMem(buffer, bufferSize));
}

void SDLMixerStreamSource::Load(SDL_RWops* rw) {
  if (!rw) {
    throw std::runtime_error(SDL2::SDL_GetError());
  }

  this->music = SDL2::mixer::Mix_LoadMUS_RW(rw, SDL_TRUE);

  if (!this->music) {
    throw std::runtime_error(SDL_GetError());
  }
}

void SDLMixerStreamSource::Unload() {
  if (this->music) {
    SDL2::mixer::Mix_FreeMusic(this->music);
    this->music = nullptr;
  }
}

void SDLMixerStreamSource::Destroy() {
  this->Unload();
}

void SDLMixerStreamSource::Play(int32_t fadeInMs, int32_t loops) {
  if (!this->music) {
    return;
  }

  int32_t result;

  if (fadeInMs > 0) {
    result = SDL2::mixer::Mix_FadeInMusic(this->music, loops - 1, fadeInMs);
  } else {
    result = SDL2::mixer::Mix_PlayMusic(this->music, loops - 1);
  }

  if (result < 0) {
    LOG_ERROR(SDL2::SDL_GetError());
  }
}

float SDLMixerStreamSource::GetVolume() {
  return 0;
}

void SDLMixerStreamSource::SetVolume(float volume) {
}

bool SDLMixerStreamSource::HasCapability(AudioSourceCapability capability) {
  switch (capability) {
    case AudioSourceCapabilityFadeIn:
    case AudioSourceCapabilityLoop:
      return true;
    default:
      return false;
  }
}

} // namespace lse
