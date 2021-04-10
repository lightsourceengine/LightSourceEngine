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
