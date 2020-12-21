/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/string-ext.h>

#include <algorithm>
#include <iterator>
#include <cctype>

namespace lse {

std::string& ToLowercase(std::string& str) noexcept {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);

  return str;
}

char* ToLowercase(char* str) noexcept {
  if (str) {
    size_t len = strlen(str);

    for (size_t i = 0; i < len; i++) {
      str[i] = static_cast<char>(::tolower(str[i]));
    }
  }

  return str;
}

} // namespace lse

namespace lse {
namespace internal {

const char* FormatArg(const bool& value) noexcept {
  return value ? "true" : "false";
}

const char* FormatArg(const std::string& value) noexcept {
  return value.c_str();
}

const char* FormatArg(std::nullptr_t) noexcept {
  return "null";
}

const char* FormatArg(const char* value) noexcept {
  return value ? value : "null";
}

const char* FormatArg(const std::exception& e) noexcept {
  return e.what();
}

} // namespace internal
} // namespace lse
