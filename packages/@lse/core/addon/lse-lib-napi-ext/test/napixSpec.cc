/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <napix.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

static napi_value CreateObject(napi_env env);
static napi_value CreateString(napi_env env, const char* text);
static std::string GetAndClearLastExceptionMessage(napi_env env);

void napixSpec(TestSuite* parent) {
  auto spec{ parent->Describe("napix functions") };

  spec->Describe("napi_throw_error()")->tests = {
    {
      "should throw an error",
      [](const TestInfo& info) {
        auto env{info.Env()};

        napix::throw_error(env, "test");
        Assert::Equal(GetAndClearLastExceptionMessage(env), std::string("test"));
      }
    },
    {
      "should not throw if there is a pending exception",
      [](const TestInfo& info) {
        auto env{info.Env()};

        napix::throw_error(env, "first");
        napix::throw_error(env, "second");
        Assert::Equal(GetAndClearLastExceptionMessage(env), std::string("first"));
      }
    }
  };

  spec->Describe("is_nullish()")->tests = {
    {
      "should return true for nullptr",
      [](const TestInfo& info) {
        auto env{info.Env()};

        Assert::IsTrue(napix::is_nullish(env, nullptr));
      }
    },
    {
      "should return true for js null",
      [](const TestInfo& info) {
        auto env{info.Env()};
        napi_value null{};
        napi_get_null(env, &null);

        Assert::IsNotNull(null);
        Assert::IsTrue(napix::is_nullish(env, null));
      }
    },
    {
      "should return true for undefined",
      [](const TestInfo& info) {
        auto env{info.Env()};
        napi_value undefined{};
        napi_get_undefined(env, &undefined);

        Assert::IsNotNull(undefined);
        Assert::IsTrue(napix::is_nullish(env, undefined));
      }
    },
    {
      "should return false for object",
      [](const TestInfo& info) {
        auto env{info.Env()};

        napi_value object = CreateObject(env);
        Assert::IsFalse(napix::is_nullish(env, object));
      }
    }
  };

  spec->Describe("is_function()")->tests = {
    {
      "should return false for nullptr",
      [](const TestInfo& info) {
        auto env{ info.Env() };

        Assert::IsFalse(napix::is_function(env, nullptr));
      }
    },
    {
      "should return true for function",
      [](const TestInfo& info) {
        auto env{ info.Env() };

        napi_value func{};
        napi_create_function(
            env,
            "func",
            0,
            [](napi_env, napi_callback_info) -> napi_value { return {}; },
            nullptr,
            &func);

        Assert::IsTrue(napix::is_function(env, func));
      }
    }
  };

  spec->Describe("as_string_utf8()")->tests = {
    {
      "should return empty string for nullptr",
      [](const TestInfo& info) {
        auto env{ info.Env() };

        Assert::Equal(napix::as_string_utf8(env, nullptr), "");
      }
    },
    {
      "should return empty string for object",
      [](const TestInfo& info) {
        auto env{ info.Env() };

        Assert::Equal(napix::as_string_utf8(env, CreateObject(env)), "");
      }
    },
    {
      "should return empty string for object",
      [](const TestInfo& info) {
        auto env{ info.Env() };

        Assert::Equal(napix::as_string_utf8(env, CreateObject(env)), "");
      }
    },
    {
      "should return string value for js string",
      [](const TestInfo& info) {
        auto env{ info.Env() };

        Assert::Equal(napix::as_string_utf8(env, CreateString(env, "test")), "test");
      }
    }
  };

  spec->Describe("copy_utf8()")->tests = {
    {
      "should copy js string into buffer",
      [](const TestInfo& info) {
        auto env{ info.Env() };
        char buffer[5];
        auto value = napix::copy_utf8(env, napix::to_value(env, "test"), buffer, 5, "");

        Assert::CStringEqual(value, "test");
      }
    },
    {
      "should return fallback when passed a js number",
      [](const TestInfo& info) {
        auto env{ info.Env() };
        char buffer[5];
        auto value = napix::copy_utf8(env, napix::to_value(env, 3), buffer, 5, "");

        Assert::CStringEqual(value, "");
      }
    },
    {
      "should return fallback if js value is nullptr",
      [](const TestInfo& info) {
        auto env{ info.Env() };
        char buffer[5];
        auto value = napix::copy_utf8(env, nullptr, buffer, 5, "");

        Assert::CStringEqual(value, "");
      }
    },
    {
      "should return fallback if js string value does not fit in buffer (exact)",
      [](const TestInfo& info) {
        auto env{ info.Env() };
        char buffer[5];
        auto value = napix::copy_utf8(env, napix::to_value(env, "12345"), buffer, 5, "");

        Assert::CStringEqual(value, "");
      }
    },
    {
      "should return fallback if js string value does not fit in buffer",
      [](const TestInfo& info) {
        auto env{ info.Env() };
        char buffer[5];
        auto value = napix::copy_utf8(env, napix::to_value(env, "123456"), buffer, 5, "");

        Assert::CStringEqual(value, "");
      }
    }
  };
}

static std::string GetAndClearLastExceptionMessage(napi_env env) {
  napi_value value{};
  napi_get_and_clear_last_exception(env, &value);

  Assert::IsNotNull(value);

  napi_value message{};
  napi_get_named_property(env, value, "message", &message);

  Assert::IsNotNull(message);

  return napix::as_string_utf8(env, message);
}

static napi_value CreateObject(napi_env env) {
  napi_value object{};
  auto status = napi_create_object(env, &object);

  Assert::IsTrue(status == napi_ok);
  Assert::IsNotNull(object);

  return object;
}

static napi_value CreateString(napi_env env, const char* text) {
  napi_value str{};
  auto status = napi_create_string_utf8(env, text, NAPI_AUTO_LENGTH, &str);

  Assert::IsTrue(status == napi_ok);
  Assert::IsNotNull(str);

  return str;
}

} // namespace lse
