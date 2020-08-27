/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/bindings/Logger.h>

using Napi::CallbackInfo;
using Napi::ClassBuilder;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::String;
using Napi::Value;

namespace ls {
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
        .WithStaticValue("Off", ls::LogLevel::LogLevelOff)
        .WithStaticValue("Error", ls::LogLevel::LogLevelError)
        .WithStaticValue("Warn", ls::LogLevel::LogLevelWarn)
        .WithStaticValue("Info", ls::LogLevel::LogLevelInfo)
        .WithStaticValue("Debug", ls::LogLevel::LogLevelDebug)
        .WithStaticValue("All", ls::LogLevel::LogLevelAll)
        .ToConstructor();
}

void Logger::LogInfo(const Napi::CallbackInfo &info) {
    Log(LogLevelInfo, info);
}

void Logger::LogError(const Napi::CallbackInfo &info) {
    Log(LogLevelError, info);
}

void Logger::LogDebug(const Napi::CallbackInfo &info) {
    Log(LogLevelDebug, info);
}

void Logger::LogWarn(const Napi::CallbackInfo &info) {
    Log(LogLevelWarn, info);
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
    return Number::New(info.Env(), ls::GetLogLevel());
}

void Logger::SetLogLevel(const CallbackInfo& info) {
    if (info[0].IsNumber()) {
        auto logLevel{ info[0].As<Number>().Int32Value() };

        if (IsLogLevel(logLevel)) {
            ls::SetLogLevel(static_cast<LogLevel>(logLevel));
        }
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

    ls::SetLogSink(nullptr);
}

} // namespace bindings
} // namespace ls
