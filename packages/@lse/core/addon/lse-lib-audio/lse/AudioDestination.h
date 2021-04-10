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
