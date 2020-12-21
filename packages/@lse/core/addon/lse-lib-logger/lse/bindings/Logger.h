/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/Log.h>

namespace lse {
namespace bindings {

/**
 * Log API exposed to javascript as static methods on a Logger class.
 */
class Logger {
 public:
    static void LogInfo(const Napi::CallbackInfo &info);
    static void LogError(const Napi::CallbackInfo &info);
    static void LogDebug(const Napi::CallbackInfo &info);
    static void LogWarn(const Napi::CallbackInfo &info);
    static void Close(const Napi::CallbackInfo &info);
    static Napi::Value GetLogLevel(const Napi::CallbackInfo &info);
    static void SetLogLevel(const Napi::CallbackInfo &info);
    static Napi::Value GetSink(const Napi::CallbackInfo &info);
    static void SetSink(const Napi::CallbackInfo &info);

 private:
    static void Log(LogLevel logLevel, const Napi::CallbackInfo &info);
};

// logger js class creators

Napi::Function NewLoggerClass(Napi::Env env);
Napi::Function NewLogLevelClass(Napi::Env env);

} // namespace bindings
} // namespace lse
