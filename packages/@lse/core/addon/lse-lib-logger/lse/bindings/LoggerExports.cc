/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "LoggerExports.h"

#include <napix.h>
#include <lse/Log.h>

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
  static constexpr auto kSiteBufferSize = 128;
  static constexpr auto kMessageBufferSize = 1024;
  static char siteBuffer[kSiteBufferSize];
  static char messageBuffer[kMessageBufferSize];

  auto ci = napix::get_callback_info<2>(env, info);

  switch (ci.length()) {
    case 2:
      LogCustomSite(
          logLevel,
          napix::copy_utf8(env, ci[1], siteBuffer, kSiteBufferSize, "js"),
          napix::copy_utf8(env, ci[0], messageBuffer, kMessageBufferSize, nullptr));
      break;
    case 1:
      LogCustomSite(
          logLevel,
          "js",
          napix::copy_utf8(env, ci[0], messageBuffer, kMessageBufferSize, nullptr));
      break;
    case 0:
      LogCustomSite(logLevel, "js", nullptr);
      break;
    default:
      break;
  }
}

static napi_value GetLogLevel(napi_env env, napi_callback_info info) noexcept {
  return napix::to_value_or_null(env, lse::GetLogLevel());
}

static napi_value SetLogLevel(napi_env env, napi_callback_info info) noexcept {
  auto ci = napix::get_callback_info<1>(env, info);
  napi_valuetype typeOf{};

  napi_typeof(env, ci[0], &typeOf);

  switch (typeOf) {
    case napi_number: {
      int32_t logLevel{ napix::as_int32(env, ci[0], -1) };

      if (IsLogLevel(logLevel)) {
        lse::SetLogLevel(static_cast<LogLevel>(logLevel));
      } else {
        napix::throw_error(env, "LogLevel value out of range.");
      }

      break;
    }
    case napi_string: {
      auto value = napix::as_string_utf8(env, ci[0]);

      if (!lse::SetLogLevel(value.c_str())) {
        napix::throw_error(env, "Invalid LogLevel value.");
      }

      break;
    }
    default:
      napix::throw_error(env, "LogLevel must be a string or integer.");
      break;
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
