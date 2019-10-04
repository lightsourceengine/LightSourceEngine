/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <fmt/format.h>

namespace fmt {

template <typename S, typename... Args, typename std::enable_if<(internal::is_string<S>::value), int>::type = 0>
inline void println(const S& format_str, const Args&... args) {
    fmt::vprint(fmt::to_string_view(format_str), fmt::internal::make_args_checked(format_str, args...));
    std::fputc('\n', stdout);
}

} // namespace fmt
