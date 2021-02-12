/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/AudioDestination.h>

namespace lse {

class SDLAudioDestination final : public AudioDestination {
 public:
  void Destroy() override;
  std::vector<std::string> GetDecoders() override;
  void Resume() override;
  void Pause() override;
  void Stop(int32_t fadeOutMs) override;
  float GetVolume() override;
  void SetVolume(float volume) override;
  bool HasCapability(AudioDestinationCapability capability) override;

 private:
  int32_t deviceId{1};
};

} // namespace lse
