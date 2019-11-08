/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {
namespace bindings {

/**
 * Log API exposed to javascript as static methods on a Logger class.
 */
class Logger : public Napi::ObjectWrap<Logger> {
 public:
    explicit Logger(const Napi::CallbackInfo &info);
    virtual ~Logger() = default;

    static Napi::Function Constructor(Napi::Env env);

 private: // javascript bindings
    static void Log(const Napi::CallbackInfo &info);
    static void Close(const Napi::CallbackInfo &info);
    static Napi::Value GetLogLevel(const Napi::CallbackInfo &info);
    static void SetLogLevel(const Napi::CallbackInfo &info);
    static Napi::Value GetSink(const Napi::CallbackInfo &info);
    static void SetSink(const Napi::CallbackInfo &info);
};

} // namespace bindings
} // namespace ls
