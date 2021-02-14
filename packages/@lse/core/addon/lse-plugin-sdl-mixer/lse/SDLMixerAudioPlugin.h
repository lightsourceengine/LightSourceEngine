/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/AudioPlugin.h>

namespace lse {

class SDLMixerAudioPlugin final : public AudioPlugin {
 public:
  explicit SDLMixerAudioPlugin(const AudioPluginConfig& config);

  void Attach() override;
  void Detach() override;
  void Destroy() override;
  std::vector<std::string> GetDevices() const noexcept override;
};

} // namespace lse
