/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

// Generates ToString<T>() and FromString<T>() mapping functions for an enum.
#define LS_ENUM_STRING_MAPPING(NAME) \
    const char* NAME##ToString(NAME) noexcept; \
    NAME NAME##FromString(const char*); \
    template <> \
    inline const char* ToString<NAME>(NAME value) noexcept { \
        return NAME##ToString(value); \
    } \
    template <> \
    inline NAME FromString<NAME>(const char* value) { \
        return NAME##FromString(value); \
    }

// Generates an enum declaration with associates ToString<T>() and FromString<T>() mapping functions.
#define LS_ENUM_SEQ_DECL(NAME, ...)  \
    enum NAME {__VA_ARGS__};         \
        template <> \
    constexpr int32_t Count<NAME>() noexcept { \
        return internal::CountMacroVariadicArgs<__VA_ARGS__>(); \
    } \
    LS_ENUM_STRING_MAPPING(NAME)

namespace ls {

template <typename T>
constexpr int32_t Count() noexcept;

template <typename T>
const char* ToString(T) noexcept;

template <typename T>
T FromString(const char*);

template <typename T>
constexpr bool IsEnum(const int32_t value) noexcept {
    return value >= 0 && value < Count<T>();
}

namespace internal {

template <int32_t... xs>
constexpr int32_t CountMacroVariadicArgs() noexcept {
    return sizeof...(xs);
}

} // namespace internal
} // namespace ls
