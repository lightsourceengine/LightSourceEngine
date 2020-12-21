/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/bindings/Logger.h>

using Napi::CallbackInfo;
using Napi::ClassBuilder;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::String;
using Napi::Value;

namespace lse {
namespace bindings {

Function NewLoggerClass(Napi::Env env) {
    return ClassBuilder(env, "Logger")
        .WithStaticMethod("info", &Logger::LogInfo)
        .WithStaticMethod("warn", &Logger::LogWarn)
        .WithStaticMethod("debug", &Logger::LogDebug)
        .WithStaticMethod("error", &Logger::LogError)
        .WithStaticMethod("getLogLevel", &Logger::GetLogLevel)
        .WithStaticMethod("setLogLevel", &Logger::SetLogLevel)
        .ToConstructor();
}

Function NewLogLevelClass(Napi::Env env) {
    return ClassBuilder(env, "LogLevel")
        .WithStaticValue("OFF", lse::LogLevel::LogLevelOff)
        .WithStaticValue("ERROR", lse::LogLevel::LogLevelError)
        .WithStaticValue("WARN", lse::LogLevel::LogLevelWarn)
        .WithStaticValue("INFO", lse::LogLevel::LogLevelInfo)
        .WithStaticValue("DEBUG", lse::LogLevel::LogLevelDebug)
        .WithStaticValue("ALL", lse::LogLevel::LogLevelAll)
        .ToConstructor();
}

void Logger::LogInfo(const Napi::CallbackInfo &info) {
    if (lse::GetLogLevel() >= LogLevelInfo) {
        Log(LogLevelInfo, info);
    }
}

void Logger::LogError(const Napi::CallbackInfo &info) {
    if (lse::GetLogLevel() >= LogLevelError) {
        Log(LogLevelError, info);
    }
}

void Logger::LogDebug(const Napi::CallbackInfo &info) {
    if (lse::GetLogLevel() >= LogLevelDebug) {
        Log(LogLevelDebug, info);
    }
}

void Logger::LogWarn(const Napi::CallbackInfo &info) {
    if (lse::GetLogLevel() >= LogLevelWarn) {
        Log(LogLevelWarn, info);
    }
}

void Logger::Log(LogLevel logLevel, const CallbackInfo& info) {
    auto env{ info.Env() };
    HandleScope scope(env);

    std::string site;
    std::string message;

    switch (info.Length()) {
        case 2:
            message = info[0].ToString();
            site = info[1].As<String>();
            break;
        case 1:
        default:
            if (info.Length() > 0) {
                message = info[0].ToString();
            }
            break;
    }

    internal::LogCustomSite(logLevel, site.empty() ? "js" : site.c_str(), message.c_str());
}

void Logger::Close(const CallbackInfo& info) {
}

Value Logger::GetLogLevel(const CallbackInfo& info) {
    return Number::New(info.Env(), lse::GetLogLevel());
}

void Logger::SetLogLevel(const CallbackInfo& info) {
    auto env{ info.Env() };

    switch (info[0].Type()) {
        case napi_number: {
            int32_t logLevel{ info[0].As<Number>() };

            if (IsLogLevel(logLevel)) {
                lse::SetLogLevel(static_cast<LogLevel>(logLevel));
            } else {
                throw Error::New(env, "LogLevel value out of range.");
            }

            break;
        }
        case napi_string: {
            auto value = Napi::CopyUtf8(info[0]);

            if (!lse::SetLogLevel(value)) {
                throw Error::New(env, "Invalid LogLevel value.");
            }

            break;
        }
        default:
            throw Error::New(env, "LogLevel must be a string or integer.");
    }
}

Value Logger::GetSink(const CallbackInfo& info) {
    auto sink{ GetLogSink() };

    if (sink == stdout) {
        return String::New(info.Env(), "stdout");
    } else {
        return info.Env().Null();
    }
}

void Logger::SetSink(const CallbackInfo& info) {
    auto env{ info.Env() };
    // TODO: use optional
    FILE* sink{ stderr };

    if (info[0].IsString()) {
        auto path{ info[0].As<String>().Utf8Value() };

        if (path == "stdout") {
            sink = stdout;
        }
    } else if (info[0].IsNull()) {
        sink = nullptr;
    }

    if (sink == stderr) {
        throw Error::New(env, "stdout or null is the only supported log sink");
    }

    lse::SetLogSink(nullptr);
}

} // namespace bindings
} // namespace lse
