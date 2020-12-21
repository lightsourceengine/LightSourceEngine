/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "Uri.h"

#include <cassert>
#include <cctype>

namespace lse {

static const char* kFilePrefix = "file:";
static const std::size_t kFilePrefixSize = strlen(kFilePrefix);

static void urldecode2(char* dst, const char* src) noexcept;
static bool Find(const std::string& str, const char* search, std::size_t start, std::size_t& pos) noexcept;
static bool Find(const std::string& str, char search, std::size_t start, std::size_t& pos) noexcept;

UriScheme GetUriScheme(const std::string& uri) noexcept {
  if (uri.rfind(kFilePrefix, 0) == 0) {
    return UriSchemeFile;
  }

  return UriSchemeUnknown;
}

std::string GetPathFromFileUri(const std::string& uri) {
  if (uri.empty()) {
    return {};
  }

  const auto end = uri.find('?');
  auto path{ uri.substr(kFilePrefixSize, end == std::string::npos ? uri.size() : end - kFilePrefixSize) };

  urldecode2(&path[0], path.c_str());
  path.resize(strlen(&path[0]));

  return path;
}

std17::string_view GetQueryParamView(const std::string& uri, const char* name) noexcept {
  assert(name != nullptr);

  auto pos = std::string::npos;

  if (!Find(uri, '?', 0, pos)) {
    return {};
  }

  const auto nameLen = strlen(name);

  while (true) {
    if (!Find(uri, name, pos, pos)) {
      return {};
    }

    pos += nameLen;

    if ((pos + 1) >= uri.size()) {
      return {};
    }

    if (uri[pos++] != '=') {
      continue;
    }

    auto end = std::string::npos;

    if (Find(uri, '&', pos, end)) {
      return { &uri[pos], end - pos };
    } else {
      return { &uri[pos], uri.size() - pos };
    }
  }
}

std::string GetQueryParam(const std::string& uri, const char* name) {
  auto value = GetQueryParamView(uri, name);

  if (value.empty()) {
    return {};
  }

  auto decodedValue = value.to_string();

  urldecode2(&decodedValue[0], decodedValue.c_str());
  decodedValue.resize(strlen(&decodedValue[0]));

  return decodedValue;
}

int32_t GetQueryParamInteger(const std::string& uri, const char* name, int32_t fallback) {
  const auto value = GetQueryParam(uri, name);

  if (value.empty()) {
    return fallback;
  }

  try {
    return std::stoi(value);
  } catch (std::exception& e) {
    return fallback;
  }
}

// from: https://stackoverflow.com/questions/2673207/c-c-url-decode-library
static void urldecode2(char* dst, const char* src) noexcept {
  char a, b;

  while (*src) {
    if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (std::isxdigit(a) && std::isxdigit(b))) {
      if (a >= 'a') {
        a -= 'a' - 'A';
      }

      if (a >= 'A') {
        a -= ('A' - 10);
      } else {
        a -= '0';
      }

      if (b >= 'a') {
        b -= 'a' - 'A';
      }

      if (b >= 'A') {
        b -= ('A' - 10);
      } else {
        b -= '0';
      }

      *dst++ = static_cast<char>(16 * a + b);
      src += 3;
    } else if (*src == '+') {
      *dst++ = ' ';
      src++;
    } else {
      *dst++ = *src++;
    }
  }

  *dst = '\0';
}

static bool Find(const std::string& str, const char* search, std::size_t start, std::size_t& pos) noexcept {
  pos = str.find(search, start);

  return (pos != std::string::npos);
}

static bool Find(const std::string& str, char search, std::size_t start, std::size_t& pos) noexcept {
  pos = str.find(search, start);

  return (pos != std::string::npos);
}

} // namespace lse
