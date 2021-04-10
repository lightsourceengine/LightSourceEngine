/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
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
    throw std::runtime_error(SDL2::SDL_GetError());
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
