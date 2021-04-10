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
#include <lse/AudioPluginConfig.h>

namespace lse {

class AudioPlugin {
 public:
  explicit AudioPlugin(const AudioPluginConfig& config);
  virtual ~AudioPlugin() = default;

 public:
  virtual void Attach() = 0;
  virtual void Detach() = 0;
  virtual void Destroy() = 0;
  virtual std::vector<std::string> GetDevices() const noexcept = 0;
  bool IsAttached() const noexcept;

 protected:
  AudioPluginConfig config{};
  bool isAttached{};
};

} // namespace lse
