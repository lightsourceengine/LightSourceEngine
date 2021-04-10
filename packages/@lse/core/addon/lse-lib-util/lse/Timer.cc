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

#include "Timer.h"
#include <lse/Log.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

namespace lse {

Timer::Timer() : start(high_resolution_clock::now()) {
}

Timer::Timer(const std::string& label) : start(high_resolution_clock::now()), label(label) {
}

void Timer::Log() const {
  const auto elapsed{
      duration_cast<std::chrono::microseconds>(high_resolution_clock::now() - this->start).count() / 1000.f
  };

  LOG_INFO("%s: %.2fms", this->label.empty() ? "elapsed" : this->label, elapsed);
}

Timer::~Timer() {
  this->Log();
}

} // namespace lse
