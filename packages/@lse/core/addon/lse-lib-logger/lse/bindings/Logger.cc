/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/bindings/Logger.h>
#include <ObjectBuilder.h>

using Napi::CallbackInfo;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectBuilder;
using Napi::String;
using Napi::Value;
using lse::internal::LogCustomSite;

namespace lse {
namespace bindings {

static void Log(LogLevel logLevel, napi_env env, napi_callback_info info);

static napi_value LogInfo(napi_env env, napi_callback_info info) {
  if (lse::GetLogLevel() >= LogLevelInfo) {
    Log(LogLevelInfo, env, info);
  }
  return {};
}

static napi_value LogError(napi_env env, napi_callback_info info) {
  if (lse::GetLogLevel() >= LogLevelError) {
    Log(LogLevelError, env, info);
  }
  return {};
}

static napi_value LogDebug(napi_env env, napi_callback_info info) {
  if (lse::GetLogLevel() >= LogLevelDebug) {
    Log(LogLevelDebug, env, info);
  }
  return {};
}

static napi_value LogWarn(napi_env env, napi_callback_info info) {
  if (lse::GetLogLevel() >= LogLevelWarn) {
    Log(LogLevelWarn, env, info);
  }
  return {};
}

static void Log(LogLevel logLevel, napi_env env, napi_callback_info info) {
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
        LogCustomSite(logLevel, site, ci[0].IsString() ? ci[0].As<Napi::String>().Utf8Value().c_str() : nullptr);
      }
    }
      break;
    case 1:
      if (Napi::StringByteLength(ci[0]) < messageBufferSize) {
        LogCustomSite(logLevel, "js", Napi::CopyUtf8(ci[0], messageBuffer, messageBufferSize));
      } else {
        LogCustomSite(logLevel, "js", ci[0].IsString() ? ci[0].As<Napi::String>().Utf8Value().c_str() : nullptr);
      }
      break;
    case 0:
      internal::LogCustomSite(logLevel, "js", nullptr);
      break;
    default:
      break;
  }
}

static napi_value GetLogLevel(napi_env env, napi_callback_info info) {
  return Number::New(env, lse::GetLogLevel());
}

static napi_value SetLogLevel(napi_env env, napi_callback_info callback_info) {
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

Object NewLoggerObject(Napi::Env env) {
  return ObjectBuilder(env)
      .WithMethod("info", &LogInfo)
      .WithMethod("warn", &LogWarn)
      .WithMethod("debug", &LogDebug)
      .WithMethod("error", &LogError)
      .WithMethod("getLogLevel", &GetLogLevel)
      .WithMethod("setLogLevel", &SetLogLevel)
      .Freeze()
      .ToObject();
}

Object NewLogLevelEnum(Napi::Env env) {
  return ObjectBuilder(env)
      .WithValue("OFF", lse::LogLevel::LogLevelOff)
      .WithValue("ERROR", lse::LogLevel::LogLevelError)
      .WithValue("WARN", lse::LogLevel::LogLevelWarn)
      .WithValue("INFO", lse::LogLevel::LogLevelInfo)
      .WithValue("DEBUG", lse::LogLevel::LogLevelDebug)
      .WithValue("ALL", lse::LogLevel::LogLevelAll)
      .Freeze()
      .ToObject();
}

} // namespace bindings
} // namespace lse
