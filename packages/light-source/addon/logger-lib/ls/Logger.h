/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>
#include <exception>
#include <initializer_list>

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

// Macro helpers.
#define _LOG_SITE() { __FILE__, __LINE__, __FUNCTION__ }
#define _LOG_SITE_LAMBDA() { __FILE__, __LINE__, LAMBDA_FUNCTION }
#define _LOG(logSite, logLevel, ...)                                 \
    if (logLevel <= ls::GetLogLevel()) {                             \
       ls::internal::Log(logSite, logLevel, __VA_ARGS__);            \
    }

// Standard logging macros.
#define LOG_DEBUG(...) _LOG(_LOG_SITE(), ls::LogLevelDebug, __VA_ARGS__)
#define LOG_INFO(...)  _LOG(_LOG_SITE(), ls::LogLevelInfo, __VA_ARGS__)
#define LOG_WARN(...)  _LOG(_LOG_SITE(), ls::LogLevelWarn, __VA_ARGS__)
#define LOG_ERROR(...) _LOG(_LOG_SITE(), ls::LogLevelError, __VA_ARGS__)

// Lambda logging macros.
// Expects a user defined LAMBDA_FUNCTION const char* to be in scope. LAMBDA_FUNCTION replaces __FUNCTION__. Lambdas
// may use the non-LAMBDA log macros, but compilers may not set __FUNCTION__ to a useful string.
#define LOG_DEBUG_LAMBDA(...) _LOG(_LOG_SITE_LAMBDA(), ls::LogLevelDebug, __VA_ARGS__)
#define LOG_INFO_LAMBDA(...)  _LOG(_LOG_SITE_LAMBDA(), ls::LogLevelInfo, __VA_ARGS__)
#define LOG_WARN_LAMBDA(...)  _LOG(_LOG_SITE_LAMBDA(), ls::LogLevelWarn, __VA_ARGS__)
#define LOG_ERROR_LAMBDA(...) _LOG(_LOG_SITE_LAMBDA(), ls::LogLevelError, __VA_ARGS__)

namespace internal {

struct LogSite {
    const char* file;
    unsigned line;
    const char* function;
};

// Internal C variadic argument signature.
void LogV(const LogSite& site, const LogLevel logLevel, const char* format, ...) noexcept;

// Internal processing variadic template arguments. For special C++ types (string, exception and bool), the values
// are forwarded to printf as C strings. All other values are forwarded to printf.
template<typename T>
const T& LogProcessArg(const T& value) noexcept {
    return value;
}
const char* LogProcessArg(const bool& value) noexcept;
const char* LogProcessArg(const std::string& value) noexcept;
const char* LogProcessArg(const std::exception& value) noexcept;

// Log methods called by LOG_* macros.
template<typename... Args>
void Log(const LogSite& site, const LogLevel logLevel, const char* format, const Args&... args) noexcept {
    LogV(site, logLevel, format, LogProcessArg(args)...);
}
void Log(const LogSite& site, const LogLevel logLevel, const std::exception& e) noexcept;
void Log(const LogSite& site, const LogLevel logLevel, const std::string& str) noexcept;

} // namespace internal

} // namespace ls
