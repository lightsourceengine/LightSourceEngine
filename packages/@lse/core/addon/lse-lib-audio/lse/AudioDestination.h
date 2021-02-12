/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <lse/EnumSequence.h>

namespace lse {

LSE_ENUM_SEQ_DECL(
    AudioDestinationCapability,
    AudioDestinationCapabilityStop,
    AudioDestinationCapabilityResume,
    AudioDestinationCapabilityPause,
    AudioDestinationCapabilityVolume,
    AudioDestinationCapabilityFadeOut)

class AudioDestination {
 public:
  virtual ~AudioDestination() = default;

  virtual void Destroy() = 0;
  virtual std::vector<std::string> GetDecoders() = 0;
  virtual void Resume() = 0;
  virtual void Pause() = 0;
  virtual void Stop(int32_t fadeOutMs) = 0;
  virtual float GetVolume() = 0;
  virtual void SetVolume(float volume) = 0;
  virtual bool HasCapability(AudioDestinationCapability capability) = 0;
};

} // namespace lse
