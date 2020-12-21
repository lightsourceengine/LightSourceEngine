/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>
#include <exception>
#include <cstdio>

namespace lse {

enum LogLevel {
    LogLevelOff,

    LogLevelError,
    LogLevelWarn,
    LogLevelInfo,
    LogLevelDebug,
    LogLevelAll,

    LogLevelCount,
};

bool IsLogLevel(int32_t logLevel) noexcept;

void SetLogLevel(LogLevel logLevel) noexcept;

bool SetLogLevel(const char* logLevel) noexcept;

LogLevel GetLogLevel() noexcept;

bool IsLogEnabled(const LogLevel logLevel) noexcept;

FILE* GetLogSink() noexcept;

void SetLogSink(FILE* file) noexcept;

// Macro helpers.
#define _LOG_SITE() { __FILE__, __LINE__, __FUNCTION__ }
#define _LOG_SITE_LAMBDA() { __FILE__, __LINE__, LAMBDA_FUNCTION }
#define _LOG(logSite, logLevel, ...)                                 \
    if (IsLogEnabled(logLevel)) {                                    \
       lse::internal::Log(logLevel, logSite, __VA_ARGS__);            \
    }

// Standard logging macros.
#define LOG_DEBUG(...) _LOG(_LOG_SITE(), lse::LogLevelDebug, __VA_ARGS__)
#define LOG_INFO(...)  _LOG(_LOG_SITE(), lse::LogLevelInfo, __VA_ARGS__)
#define LOG_WARN(...)  _LOG(_LOG_SITE(), lse::LogLevelWarn, __VA_ARGS__)
#define LOG_ERROR(...) _LOG(_LOG_SITE(), lse::LogLevelError, __VA_ARGS__)

// Lambda logging macros.
// Expects a user defined LAMBDA_FUNCTION const char* to be in scope. LAMBDA_FUNCTION replaces __FUNCTION__. Lambdas
// may use the non-LAMBDA log macros, but compilers may not set __FUNCTION__ to a useful string.
#define LOG_DEBUG_LAMBDA(...) _LOG(_LOG_SITE_LAMBDA(), lse::LogLevelDebug, __VA_ARGS__)
#define LOG_INFO_LAMBDA(...)  _LOG(_LOG_SITE_LAMBDA(), lse::LogLevelInfo, __VA_ARGS__)
#define LOG_WARN_LAMBDA(...)  _LOG(_LOG_SITE_LAMBDA(), lse::LogLevelWarn, __VA_ARGS__)
#define LOG_ERROR_LAMBDA(...) _LOG(_LOG_SITE_LAMBDA(), lse::LogLevelError, __VA_ARGS__)

// Log macros to log without site (file and function) information.
#define LOGX_DEBUG(...) _LOG({}, lse::LogLevelDebug, __VA_ARGS__)
#define LOGX_INFO(...)  _LOG({}, lse::LogLevelInfo, __VA_ARGS__)
#define LOGX_WARN(...)  _LOG({}, lse::LogLevelWarn, __VA_ARGS__)
#define LOGX_ERROR(...) _LOG({}, lse::LogLevelError, __VA_ARGS__)

namespace internal {

struct LogSite {
    const char* file{};
    unsigned line{};
    const char* function{};
};

// Helper method to print the timestamp, log site info and logLevel.
void LogPrintHeader(const LogLevel logLevel, const LogSite& site, const bool hasFormat);

// Internal processing variadic template arguments. For special C++ types (string, exception and bool), the values
// are forwarded to printf as C strings. All other values are forwarded to printf.
template<typename T>
const T& LogProcessArg(const T& value) noexcept {
    return value;
}
const char* LogProcessArg(const bool& value) noexcept;
const char* LogProcessArg(const std::string& value) noexcept;
const char* LogProcessArg(const std::exception& value) noexcept;
const char* LogProcessArg(const char* value) noexcept;
const char* LogProcessArg(std::nullptr_t) noexcept;

// Log methods called by LOG_* macros.
template<typename... Args>
void Log(const LogLevel logLevel, const LogSite& site, const char* format, const Args&... args) noexcept {
    const auto hasMessage{ format && format[0] != '\0' };

    LogPrintHeader(logLevel, site, hasMessage);

    if (hasMessage) {
        std::printf(format, LogProcessArg(args)...);
    }

    std::putc('\n', stdout);
}
void Log(const LogLevel logLevel, const LogSite& site, const std::exception& e) noexcept;
void Log(const LogLevel logLevel, const LogSite& site, const std::string& str) noexcept;
void Log(const LogLevel logLevel, const LogSite& site, const char* str) noexcept;

// Log method for javascript bindings.
void LogCustomSite(const LogLevel logLevel, const char* site, const char* message) noexcept;

} // namespace internal

} // namespace lse
