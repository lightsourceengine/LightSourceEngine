/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Timer.h"
#include <fmt/println.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

namespace ls {

Timer::Timer() : start(high_resolution_clock::now()) {
}

Timer::Timer(const std::string& label) : start(high_resolution_clock::now()), label(label) {
}

void Timer::Log() const {
    const auto elapsed{
        duration_cast<std::chrono::microseconds>(high_resolution_clock::now() - this->start).count() / 1000.f
    };

    fmt::println("{}: {:.2f}ms", this->label.empty() ? "elapsed" : this->label, elapsed);
}

Timer::~Timer() {
    this->Log();
}

} // namespace ls
