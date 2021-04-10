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
