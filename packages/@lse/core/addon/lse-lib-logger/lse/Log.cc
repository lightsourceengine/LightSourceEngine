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

#include "Log.h"
#include <chrono>
#include <ctime>
#include <cassert>

namespace lse {

static auto sLogLevel{ LogLevelInfo };
static auto sSink{ stdout };

static bool eqi(const char* a, const char* b) {
  while (::tolower(static_cast<unsigned char>(*a)) == ::tolower(static_cast<unsigned char>(*b++))) {
    if (*a++ == 0) {
      return true;
    }
  }
  return false;
}

constexpr const char* LogLevelToString(const LogLevel logLevel) noexcept {
  switch (logLevel) {
    case LogLevelDebug:
      return "DEBUG";
    case LogLevelInfo:
      return "INFO";
    case LogLevelWarn:
      return "WARN";
    case LogLevelError:
      return "ERROR";
    case LogLevelAll:
      return "ALL";
    case LogLevelOff:
      return "OFF";
    default:
      return "UNKNOWN";
  }
}

bool IsLogLevel(const int32_t logLevel) noexcept {
  switch (logLevel) {
    case LogLevelDebug:
    case LogLevelInfo:
    case LogLevelWarn:
    case LogLevelError:
    case LogLevelAll:
    case LogLevelOff:
      return true;
    default:
      return false;
  }
}

void SetLogLevel(LogLevel logLevel) noexcept {
  sLogLevel = logLevel;
}

bool SetLogLevel(const char* logLevel) noexcept {
  LogLevel e;

  if (logLevel == nullptr || *logLevel == '\0') {
    return false;
  } else if (logLevel[1] == '\0') {
    const int32_t logLevelInt = logLevel[0] - '0';

    if (!IsLogLevel(logLevelInt)) {
      return false;
    }

    e = static_cast<LogLevel>(logLevelInt);
  } else if (eqi(logLevel, LogLevelToString(LogLevelOff))) {
    e = LogLevelOff;
  } else if (eqi(logLevel, LogLevelToString(LogLevelInfo))) {
    e = LogLevelInfo;
  } else if (eqi(logLevel, LogLevelToString(LogLevelWarn))) {
    e = LogLevelWarn;
  } else if (eqi(logLevel, LogLevelToString(LogLevelError))) {
    e = LogLevelError;
  } else if (eqi(logLevel, LogLevelToString(LogLevelDebug))) {
    e = LogLevelDebug;
  } else if (eqi(logLevel, LogLevelToString(LogLevelAll))) {
    e = LogLevelAll;
  } else {
    return false;
  }

  SetLogLevel(e);

  return true;
}

LogLevel GetLogLevel() noexcept {
  return sLogLevel;
}

bool IsLogEnabled(const LogLevel logLevel) noexcept {
  return sSink && logLevel <= sLogLevel;
}

FILE* GetLogSink() noexcept {
  return sSink;
}

void SetLogSink(FILE* file) noexcept {
  if (file == stdout || file == nullptr) {
    sSink = file;
  } else {
    // TODO: support arbitrary file pointers.
    assert(false);
  }
}

namespace internal {

static
const char* GetFileBasename(const char* file) noexcept {
  const char* str{ file };
  const char* basename{ nullptr };

  do {
    if (*str == '/' || *str == '\\') {
      basename = str;
    }
  } while (*str++);

  return basename ? basename + 1 : file;
}

static
const char* TimestampNow(char* buffer, const std::size_t len) noexcept {
  const auto now{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };

  std::strftime(buffer, len, "%Y-%m-%d %H:%M:%S", std::localtime(&now));

  return buffer;
}

void Log(const LogLevel logLevel, const LogSite& site, const std::exception& e) noexcept {
  LogPrintHeader(logLevel, site, true);
  std::puts(e.what());
}

void Log(const LogLevel logLevel, const LogSite& site, const std::string& str) noexcept {
  LogPrintHeader(logLevel, site, !str.empty());
  std::puts(str.c_str());
}

void Log(const LogLevel logLevel, const LogSite& site, const char* str) noexcept {
  LogPrintHeader(logLevel, site, *str != '\0');
  std::puts(str);
}

const char* LogProcessArg(const char* value) noexcept {
  return value ? value : "null";
}

const char* LogProcessArg(std::nullptr_t) noexcept {
  return "null";
}

void LogPrintHeader(const LogLevel logLevel, const LogSite& site, const bool hasMessage) {
  char buffer[31];

  if (site.file) {
    std::printf("%s [%-5s] %s:%u %s()%s",
                TimestampNow(buffer, sizeof(buffer)),
                LogLevelToString(logLevel),
                GetFileBasename(site.file),
                site.line,
                site.function,
                hasMessage ? " - " : "");
  } else {
    std::printf("%s [%-5s]%s",
                TimestampNow(buffer, sizeof(buffer)),
                LogLevelToString(logLevel),
                hasMessage ? " " : "");
  }
}

const char* LogProcessArg(const std::string& value) noexcept {
  return value.c_str();
}

const char* LogProcessArg(const std::exception& value) noexcept {
  return value.what();
}

const char* LogProcessArg(const bool& value) noexcept {
  return value ? "true" : "false";
}

void LogCustomSite(const LogLevel logLevel, const char* site, const char* message) noexcept {
  char buffer[31];
  auto hasMessage{ message && *message != '\0' };

  std::printf("%s [%-5s] %s%s",
              TimestampNow(buffer, sizeof(buffer)),
              LogLevelToString(logLevel),
              site,
              hasMessage ? " - " : "");

  if (hasMessage) {
    std::puts(message);
  } else {
    std::putc('\n', stdout);
  }
}

} // namespace internal

} // namespace lse
