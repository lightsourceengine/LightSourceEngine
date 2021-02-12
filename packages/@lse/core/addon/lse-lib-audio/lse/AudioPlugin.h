/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <vector>
#include <string>

namespace lse {

class AudioPlugin {
 public:
  virtual ~AudioPlugin() = default;

 public:
  virtual void Attach() = 0;
  virtual void Detach() = 0;
  virtual void Destroy() = 0;
  virtual std::vector<std::string> GetDevices() const noexcept = 0;
  bool IsAttached() const noexcept;

 protected:
  bool isAttached{};
};

} // namespace lse
