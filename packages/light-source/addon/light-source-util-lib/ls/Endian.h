/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

namespace ls {

// Implementation from C++20 endian proposal: http://howardhinnant.github.io/endian.html
enum class endian {
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

/**
 * Check if ths CPU is big endian.
 */
constexpr bool IsBigEndian() {
    return endian::native == endian::big;
}

} // namespace ls
