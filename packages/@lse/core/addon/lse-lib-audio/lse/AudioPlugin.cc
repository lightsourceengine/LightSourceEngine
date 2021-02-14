/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "AudioPlugin.h"

namespace lse {

AudioPlugin::AudioPlugin(const AudioPluginConfig& config) : config(config) {
}

bool AudioPlugin::IsAttached() const noexcept {
  return this->isAttached;
}

} // namespace lse
