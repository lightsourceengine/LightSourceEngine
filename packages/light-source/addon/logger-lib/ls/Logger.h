/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>
#include <exception>

namespace ls {

enum LogLevel {
    LogLevelError,
    LogLevelWarn,
    LogLevelInfo,
    LogLevelDebug,

    LogLevelOff,
};

void SetLogLevel(const LogLevel logLevel) noexcept;

LogLevel GetLogLevel() noexcept;

#define _LOG_SITE() { __FILE__, __LINE__, __FUNCTION__ }

#define LOG_DEBUG(...)                                                        \
    if (ls::LogLevelDebug <= ls::GetLogLevel()) {                             \
        ls::internal::Log(_LOG_SITE(), ls::LogLevelDebug, __VA_ARGS__);       \
    }

#define LOG_INFO(...)                                                         \
    if (ls::LogLevelInfo <= ls::GetLogLevel()) {                              \
        ls::internal::Log(_LOG_SITE(), ls::LogLevelInfo, __VA_ARGS__);        \
    }

#define LOG_WARN(...)                                                         \
    if (ls::LogLevelWarn <= ls::GetLogLevel()) {                              \
        ls::internal::Log(_LOG_SITE(), ls::LogLevelWarn, __VA_ARGS__);        \
    }

#define LOG_ERROR(...)                                                        \
    if (ls::LogLevelError <= ls::GetLogLevel()) {                             \
        ls::internal::Log(_LOG_SITE(), ls::LogLevelError, __VA_ARGS__);       \
    }

namespace internal {

struct LogSite {
    const char* file;
    unsigned line;
    const char* function;
};

void Log(const LogSite& site, const LogLevel logLevel, const char* format, ...);
void Log(const LogSite& site, const LogLevel logLevel, const std::exception& e);
void Log(const LogSite& site, const LogLevel logLevel, const std::string& e);

} // namespace internal

} // namespace ls
