/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
