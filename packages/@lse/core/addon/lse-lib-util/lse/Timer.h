/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <chrono>
#include <string>

namespace lse {

class Timer {
 private:
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

 public:
  Timer();
  explicit Timer(const std::string& label);
  ~Timer();

  void Log() const;

 private:
  TimePoint start;
  std::string label;
};

} // namespace lse
