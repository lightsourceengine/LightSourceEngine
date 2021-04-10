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

#include <cstdint>

// Generates ToString<T>() and FromString<T>() mapping functions for an enum.
#define LSE_ENUM_STRING_MAPPING(NAME) \
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
#define LSE_ENUM_SEQ_DECL(NAME, ...)  \
    enum NAME {__VA_ARGS__};         \
        template <> \
    constexpr int32_t Count<NAME>() noexcept { \
        return internal::CountMacroVariadicArgs<__VA_ARGS__>(); \
    } \
    LSE_ENUM_STRING_MAPPING(NAME)

namespace lse {

template<typename T>
constexpr int32_t Count() noexcept;

template<typename T>
const char* ToString(T) noexcept;

template<typename T>
T FromString(const char*);

template<typename T>
constexpr bool IsEnum(const int32_t value) noexcept {
  return value >= 0 && value < Count<T>();
}

namespace internal {

template<int32_t... xs>
constexpr int32_t CountMacroVariadicArgs() noexcept {
  return sizeof...(xs);
}

} // namespace internal
} // namespace lse
