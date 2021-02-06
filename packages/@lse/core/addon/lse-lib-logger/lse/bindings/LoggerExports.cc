/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "LoggerExports.h"

#include <napix.h>
#include <napi-ext.h>
#include <lse/Log.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::String;
using Napi::Value;
using lse::internal::LogCustomSite;

using napix::object_new;
using napix::descriptor::instance_value;
using napix::descriptor::instance_method;

namespace lse {
namespace bindings {

static void Log(LogLevel logLevel, napi_env env, napi_callback_info info) noexcept;

static napi_value LogInfo(napi_env env, napi_callback_info info) noexcept {
  if (lse::GetLogLevel() >= LogLevelInfo) {
    Log(LogLevelInfo, env, info);
  }
  return {};
}

static napi_value LogError(napi_env env, napi_callback_info info) noexcept {
  if (lse::GetLogLevel() >= LogLevelError) {
    Log(LogLevelError, env, info);
  }
  return {};
}

static napi_value LogDebug(napi_env env, napi_callback_info info) noexcept {
  if (lse::GetLogLevel() >= LogLevelDebug) {
    Log(LogLevelDebug, env, info);
  }
  return {};
}

static napi_value LogWarn(napi_env env, napi_callback_info info) noexcept {
  if (lse::GetLogLevel() >= LogLevelWarn) {
    Log(LogLevelWarn, env, info);
  }
  return {};
}

static void Log(LogLevel logLevel, napi_env env, napi_callback_info info) noexcept {
  constexpr auto siteBufferSize = 32;
  constexpr auto messageBufferSize = 1024;

  static char siteBuffer[siteBufferSize];
  static char messageBuffer[messageBufferSize];

  Napi::CallbackInfo ci(env, info);

  switch (ci.Length()) {
    case 2: {
      auto site = Napi::StringByteLength(ci[1]) < siteBufferSize ?
          Napi::CopyUtf8(ci[1], siteBuffer, siteBufferSize) : "js";

      if (Napi::StringByteLength(ci[0]) < messageBufferSize) {
        LogCustomSite(logLevel, site, Napi::CopyUtf8(ci[0], messageBuffer, messageBufferSize));
      } else {
        auto message{ napix::as_string_utf8(env, ci[0]) };
        LogCustomSite(logLevel, site, !message.empty() ? message.c_str() : nullptr);
      }
    }
      break;
    case 1:
      if (Napi::StringByteLength(ci[0]) < messageBufferSize) {
        LogCustomSite(logLevel, "js", Napi::CopyUtf8(ci[0], messageBuffer, messageBufferSize));
      } else {
        auto message{ napix::as_string_utf8(env, ci[0]) };
        LogCustomSite(logLevel, "js", !message.empty() ? message.c_str() : nullptr);
      }
      break;
    case 0:
      internal::LogCustomSite(logLevel, "js", nullptr);
      break;
    default:
      break;
  }
}

static napi_value GetLogLevel(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, lse::GetLogLevel());
}

static void SetLogLevelThrows(napi_env env, napi_callback_info callback_info) {
  Napi::CallbackInfo info(env, callback_info);

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

static napi_value SetLogLevel(napi_env env, napi_callback_info callback_info) noexcept {
  NAPI_TRY_C(SetLogLevelThrows(env, callback_info))
  return {};
}

//Value Logger::GetSink(const CallbackInfo& info) {
//  auto sink{ GetLogSink() };
//
//  if (sink == stdout) {
//    return String::New(info.Env(), "stdout");
//  } else {
//    return info.Env().Null();
//  }
//}
//
//void Logger::SetSink(const CallbackInfo& info) {
//  auto env{ info.Env() };
//  // TODO: use optional
//  FILE* sink{ stderr };
//
//  if (info[0].IsString()) {
//    auto path{ info[0].As<String>().Utf8Value() };
//
//    if (path == "stdout") {
//      sink = stdout;
//    }
//  } else if (info[0].IsNull()) {
//    sink = nullptr;
//  }
//
//  if (sink == stderr) {
//    throw Error::New(env, "stdout or null is the only supported log sink");
//  }
//
//  lse::SetLogSink(nullptr);
//}

napi_value NewLoggerObject(napi_env env) noexcept {
  return object_new(env, {
      instance_method("info", &LogInfo),
      instance_method("warn", &LogWarn),
      instance_method("debug", &LogDebug),
      instance_method("error", &LogError),
      instance_method("getLogLevel", &GetLogLevel),
      instance_method("setLogLevel", &SetLogLevel),
  });
}

napi_value NewLogLevelEnum(napi_env env) noexcept {
  return object_new(env, {
      instance_value(env, "OFF", LogLevelOff, napi_enumerable),
      instance_value(env, "ERROR", LogLevelError, napi_enumerable),
      instance_value(env, "WARN", LogLevelWarn, napi_enumerable),
      instance_value(env, "INFO", LogLevelInfo, napi_enumerable),
      instance_value(env, "DEBUG", LogLevelDebug, napi_enumerable),
      instance_value(env, "ALL", LogLevelAll, napi_enumerable),
  });
}

} // namespace bindings
} // namespace lse
