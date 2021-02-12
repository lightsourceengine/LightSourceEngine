/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "SDLAudioSource.h"

#include <stdexcept>

namespace lse {

void SDLAudioSource::Load(const char* filename) {
  if (this->audioBuffer) {
    throw std::runtime_error("source already loaded");
  }

  this->Load(SDL2::SDL_RWFromFile(filename, "rb"));
}

void SDLAudioSource::Load(void* buffer, size_t bufferSize) {
  if (this->audioBuffer) {
    throw std::runtime_error("source already loaded");
  }

  this->Load(SDL2::SDL_RWFromMem(buffer, bufferSize));
}

void SDLAudioSource::Load(SDL_RWops* rw) {
  if (!rw) {
    throw std::runtime_error(SDL2::SDL_GetError());
  }

  SDL_AudioSpec spec{};
  auto result{ SDL2::SDL_LoadWAV_RW(rw, SDL_TRUE, &spec, &this->audioBuffer, &this->audioBufferLen) };

  if (!result) {
    throw std::runtime_error(SDL2::SDL_GetError());
  }
}

void SDLAudioSource::Unload() {
  if (this->audioBuffer) {
    SDL2::SDL_FreeWAV(this->audioBuffer);
    this->audioBuffer = nullptr;
    this->audioBufferLen = 0;
  }
}

void SDLAudioSource::Destroy() {
  this->Unload();
}

void SDLAudioSource::Play(int32_t fadeInMs, int32_t loops) {
  if (this->audioBuffer) {
    SDL2::SDL_ClearQueuedAudio(this->deviceId);
    SDL2::SDL_QueueAudio(this->deviceId, this->audioBuffer, this->audioBufferLen);
  }
}

float SDLAudioSource::GetVolume() {
  return 0;
}

void SDLAudioSource::SetVolume(float volume) {
}

bool SDLAudioSource::HasCapability(AudioSourceCapability capability) {
  return false;
}

} // namespace lse
