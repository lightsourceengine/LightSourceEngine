/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>
#include <cstring>

namespace lse {

namespace detail {

template<class T>
struct CStringMapEqualTo : public std::binary_function<T, T, bool> {
    bool operator()(const T &lhs, const T &rhs) const noexcept { return strcmp(lhs, rhs) == 0; }
};

struct CStringMapHash {
    int32_t operator()(const char* str) const {
        int32_t h = 0;
        while (*str) {
            h = h * 31 + static_cast<int32_t>(*str++);
        }
        return h;
    }
};

} // namespace detail

// unordered_map with const char* as the key type.
template<typename T>
using CStringHashMap
    = std::unordered_map<const char*, T, detail::CStringMapHash, detail::CStringMapEqualTo<const char*>>;

} // namespace lse
