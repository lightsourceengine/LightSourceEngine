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
#include <utf8.h>

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

bool StartsWith(const char* str, const char* prefix) noexcept {
  if (!str || !prefix) {
    return false;
  }

  return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool StartsWith(const std::string& str, const char* prefix) noexcept {
  return StartsWith(str.c_str(), prefix);
}

bool EndsWith(const char* str, const char* suffix) noexcept {
  if (!str || !suffix) {
    return false;
  }

  auto lenstr = strlen(str);
  auto lensuffix = strlen(suffix);

  if (lensuffix > lenstr) {
    return false;
  }

  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

bool EqualsIgnoreCase(const std::string& a, const char* b) noexcept {
  return EqualsIgnoreCase(a.c_str(), b);
}

bool EqualsIgnoreCase(const char* a, const char* b) noexcept {
  if (!a) {
    a = "";
  }

  if (!b) {
    b = "";
  }

#ifdef _WIN32
#ifdef __GNUC__
  while (::tolower(static_cast<uint8_t>(*a)) == ::tolower(static_cast<uint8_t>(*b++))) {
    if (*a++ == 0) {
      return true;
    }
  }
  return false;
#else
  return 0 == ::_stricmp(a, b);
#endif
#else
  return 0 == ::strcasecmp(a, b);
#endif
}

size_t LengthUtf8(const std::string& utf8) noexcept {
  auto i{ utf8.begin() };
  std::size_t length{};

  while (i != utf8.end()) {
    utf8::unchecked::next(i);
    length++;
  }

  return length;
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
