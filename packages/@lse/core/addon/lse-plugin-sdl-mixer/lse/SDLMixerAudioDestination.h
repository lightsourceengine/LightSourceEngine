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

#include <lse/AudioDestination.h>
#include <lse/SDLMixerCommon.h>

namespace lse {

class SDLMixerSample final : public AudioDestination {
 public:
  void Destroy() override;
  std::vector<std::string> GetDecoders() override;
  void Resume() override;
  void Pause() override;
  void Stop(int32_t fadeOutMs) override;
  float GetVolume() override;
  void SetVolume(float volume) override;
  bool HasCapability(AudioDestinationCapability capability) override;
};

class SDLMixerStream final : public AudioDestination {
 public:
  void Destroy() override;
  std::vector<std::string> GetDecoders() override;
  void Resume() override;
  void Pause() override;
  void Stop(int32_t fadeOutMs) override;
  float GetVolume() override;
  void SetVolume(float volume) override;
  bool HasCapability(AudioDestinationCapability capability) override;
};

} // namespace lse
