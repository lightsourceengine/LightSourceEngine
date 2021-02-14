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
    char* current = str;

    while (*current != '\0') {
      *current = static_cast<char>(::tolower(*current));
      current++;
    }
  }

  return str;
}

bool EqualsIgnoreCase(const std::string& a, const char* b) noexcept {
  const auto len = a.size();

  if (!b || b[0] == '\0') {
    return len == 0;
  }

  if (len == 0) {
    return false;
  }

  for (size_t i = 0; i < len; i++) {
    if (b[i] == '\0' || tolower(a[i]) != tolower(b[i])) {
      return false;
    }
  }

  return true;
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
