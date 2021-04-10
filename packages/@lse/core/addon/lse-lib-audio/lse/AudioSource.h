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

#include <cstdint>
#include <cstddef>
#include <lse/EnumSequence.h>

namespace lse {

LSE_ENUM_SEQ_DECL(
    AudioSourceCapability,
    AudioSourceCapabilityVolume,
    AudioSourceCapabilityLoop,
    AudioSourceCapabilityFadeIn)

class AudioSource {
 public:
  virtual void Load(const char* filename) = 0;
  virtual void Load(void* buffer, size_t bufferSize) = 0;
  virtual void Unload() = 0;
  virtual void Destroy() = 0;
  virtual void Play(int32_t fadeInMs, int32_t loops) = 0;
  virtual float GetVolume() = 0;
  virtual void SetVolume(float volume) = 0;
  virtual bool HasCapability(AudioSourceCapability capability) = 0;
};

} // namespace lse
