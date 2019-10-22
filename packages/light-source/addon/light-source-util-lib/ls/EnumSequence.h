/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

#define LS_ENUM_SEQ_DECL(NAME, ...)  \
    enum NAME {__VA_ARGS__}; \
    const char* NAME##ToString(NAME); \
    template <> \
    constexpr int32_t Count<NAME>() { \
        return internal::CountMacroVariadicArgs<__VA_ARGS__>(); \
    }

namespace ls {

template <typename T>
constexpr int32_t Count();

template <typename T>
constexpr bool IsEnum(const int32_t value) {
    return value >= 0 && value < Count<T>();
}

namespace internal {

template <int32_t... xs>
constexpr int32_t CountMacroVariadicArgs() {
    return sizeof...(xs);
}

} // namespace internal
} // namespace ls
