/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Logger.h"
#include <cstdio>
#include <iostream>
#include <chrono>
#include <ctime>

namespace ls {

static auto sLogLevel{ LogLevelInfo };

static
const char* LogLevelToString(const LogLevel logLevel) {
    switch (logLevel) {
        case LogLevelDebug:
            return "DEBUG";
        case LogLevelInfo:
            return "INFO";
        case LogLevelWarn:
            return "WARN";
        case LogLevelError:
            return "ERROR";
        case LogLevelOff:
            return "OFF";
    }

    return "UNKNOWN";
}

void SetLogLevel(const LogLevel logLevel) noexcept {
    sLogLevel = logLevel;
}

LogLevel GetLogLevel() noexcept {
    return sLogLevel;
}

namespace internal {

static
const char* GetFileBasename(const LogSite& site) {
    const char* str{ site.file };
    const char* basename{ nullptr };

    do {
        if (*str == '/' || *str == '\\') {
            basename = str;
        }
    } while (*str++);

    return basename ? basename + 1 : site.file;
}

void Log(const LogSite& site, const LogLevel logLevel, const std::exception& e) {
    Log(site, logLevel, e.what());
}

void Log(const LogSite& site, const LogLevel logLevel, const std::string& e) {
    Log(site, logLevel, e.c_str());
}

void Log(const LogSite& site, const LogLevel logLevel, const char* format, ...) {
    char timestamp[31];
    const auto now{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
    const auto sep{ format && *format != '\0' ? " - " : " "};

    std::strftime(&timestamp[0], sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::printf("%s [%-5s] %s:%u %s()%s",
        timestamp,
        LogLevelToString(logLevel),
        GetFileBasename(site),
        site.line,
        site.function,
        sep);

    va_list argptr;
    va_start(argptr, format);
    std::vprintf(format, argptr);
    va_end(argptr);

    std::puts("");
}

} // namespace internal

} // namespace ls
