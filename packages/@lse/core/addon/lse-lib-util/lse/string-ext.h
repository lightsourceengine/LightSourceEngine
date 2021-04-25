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

#include <string>
#include <phmap.h>

namespace lse {

// Simple ascii std::string conversion to lowercase, no intl or encoding considered.
std::string& ToLowercase(std::string& str) noexcept;

// Simple ascii cstring conversion to lowercase, no intl or encoding considered. If str is null, null is returned.
char* ToLowercase(char* str) noexcept;

// Case insensitive string compare.
bool EqualsIgnoreCase(const std::string& a, const char* b) noexcept;
bool EqualsIgnoreCase(const char* a, const char* b) noexcept;

// Checks if str starts with a prefix.
bool StartsWith(const char* str, const char* prefix) noexcept;
bool StartsWith(const std::string& str, const char* prefix) noexcept;

// Checks if str ends with a suffix.
bool EndsWith(const char* str, const char* suffix) noexcept;

// Get the length of a utf8 encoded string.
size_t LengthUtf8(const std::string& utf8) noexcept;

namespace internal {

template<typename T>
const T& FormatArg(const T& value) noexcept {
  return value;
}
const char* FormatArg(const bool& value) noexcept;
const char* FormatArg(const std::string& value) noexcept;
const char* FormatArg(const char* value) noexcept;
const char* FormatArg(const std::exception& e) noexcept;
const char* FormatArg(std::nullptr_t) noexcept;

} // namespace internal

/**
 * Format an std::string using printf semantics.
 *
 * In addition to full printf support, Format can handle std::string, null char*, nullptr and bool.
 */
template<typename... Args>
std::string Format(const char* format, const Args& ... args) {
  // Measure the string. size does not include the null-terminated character.
  const auto size{ snprintf(nullptr, 0, format, internal::FormatArg(args)...) };

  if (size <= 0) {
    return "";
  }

  std::string formattedString(size, '-');

  // sprintf always writes a null terminator. std::string internal buffer is size + 1 to account for the terminator.
  snprintf(&formattedString[0], size + 1, format, internal::FormatArg(args)...);

  return formattedString;
}

namespace detail {

struct CStringMapEqualTo {
  bool operator()(const char* lhs, const char* rhs) const noexcept {
    return strcmp(lhs, rhs) == 0;
  }
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
using CStringHashMap = phmap::flat_hash_map<const char*, T, detail::CStringMapHash, detail::CStringMapEqualTo>;

} // namespace lse
