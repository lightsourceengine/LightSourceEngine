/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/AudioSource.h>
#include <lse/SDL2.h>

namespace lse {

class SDLAudioSource final : public AudioSource {
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
  uint8_t* audioBuffer{};
  uint32_t audioBufferLen{};
  int32_t deviceId{1};
};

} // namespace lse
