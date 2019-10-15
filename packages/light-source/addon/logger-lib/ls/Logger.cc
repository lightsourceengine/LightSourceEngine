/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Logger.h"
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
const char* GetFileBasename(const LogSite& site) noexcept {
    const char* str{ site.file };
    const char* basename{ nullptr };

    do {
        if (*str == '/' || *str == '\\') {
            basename = str;
        }
    } while (*str++);

    return basename ? basename + 1 : site.file;
}

void Log(const LogSite& site, const LogLevel logLevel, const std::exception& e) noexcept {
    LogPrintHeader(site, logLevel, true);
    std::puts(e.what());
}

void Log(const LogSite& site, const LogLevel logLevel, const std::string& str) noexcept {
    LogPrintHeader(site, logLevel, !str.empty());
    std::puts(str.c_str());
}

void Log(const LogSite& site, const LogLevel logLevel, const char* str) noexcept {
    LogPrintHeader(site, logLevel, *str != '\0');
    std::puts(str);
}

void LogPrintHeader(const LogSite& site, const LogLevel logLevel, const bool hasFormat) {
    char timestamp[31];
    const auto now{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
    const auto sep{ hasFormat ? " - " : " "};

    std::strftime(&timestamp[0], sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::printf("%s [%-5s] %s:%u %s()%s",
        timestamp,
        LogLevelToString(logLevel),
        GetFileBasename(site),
        site.line,
        site.function,
        sep);
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

} // namespace internal

} // namespace ls
