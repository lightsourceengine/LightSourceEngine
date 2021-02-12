/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/AudioSource.h>
#include <lse/SDLMixerCommon.h>

namespace lse {

class SDLMixerSampleSource final : public AudioSource {
 public:
  void Load(const char* filename) override;
  void Load(void* buffer, size_t bufferSize) override;
  void Unload() override;
  void Destroy() override;
  void Play(int32_t fadeInMs, int32_t loops) override;
  float GetVolume() override;
  void SetVolume(float volume) override;
  bool HasCapability(AudioSourceCapability capability) override;

 private:
  void Load(SDL_RWops* rw);

 private:
  Mix_Chunk* chunk{};
};

class SDLMixerStreamSource final : public AudioSource {
 public:
  void Load(const char* filename) override;
  void Load(void* buffer, size_t bufferSize) override;
  void Unload() override;
  void Destroy() override;
  void Play(int32_t fadeInMs, int32_t loops) override;
  float GetVolume() override;
  void SetVolume(float volume) override;
  bool HasCapability(AudioSourceCapability capability) override;

 private:
  void Load(SDL_RWops* rw);

 private:
  Mix_Music* music{};
};

} // namespace lse
