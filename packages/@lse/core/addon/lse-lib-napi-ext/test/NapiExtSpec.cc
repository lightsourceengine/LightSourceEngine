/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <napi-ext.h>
#include <vector>

using Napi::Assert;
using Napi::Call;
using Napi::Error;
using Napi::Function;
using Napi::FunctionReference;
using Napi::NewStringArray;
using Napi::Number;
using Napi::RunScript;
using Napi::String;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

void NapiExtSpec(TestSuite* parent) {
  auto spec{ parent->Describe("napi-ext") };

  spec->Describe("NewStringArray()")->tests = {
      {
          "should create an Array from a list of strings",
          [](const TestInfo& info) {
            std::vector<std::string> strings{ "test1", "test2" };
            auto array{ NewStringArray(info.Env(), strings) };

            Assert::Equal(array.Length(), static_cast<uint32_t>(strings.size()));
            Assert::Equal(array.Get(0u).As<String>().Utf8Value(), strings[0]);
            Assert::Equal(array.Get(1u).As<String>().Utf8Value(), strings[1]);
          }
      }
  };

  spec->Describe("ToUpperCase()")->tests = {
      {
          "should convert string to uppercase",
          [](const TestInfo& info) {
            auto result{ ToUpperCase(String::New(info.Env(), "test")) };

            Assert::Equal(result, std::string("TEST"));
          }
      },
      {
          "should handle empty string",
          [](const TestInfo&) {
            auto result{ ToUpperCase(String()) };

            Assert::Equal(result, std::string(""));
          }
      }
  };

  spec->Describe("ToLowerCase()")->tests = {
      {
          "should convert string to uppercase",
          [](const TestInfo& info) {
            auto result{ ToLowerCase(String::New(info.Env(), "TEST")) };

            Assert::Equal(result, std::string("test"));
          }
      },
      {
          "should handle empty string",
          [](const TestInfo&) {
            auto result{ ToLowerCase(String()) };

            Assert::Equal(result, std::string(""));
          }
      }
  };

  spec->Describe("Call()")->tests = {
      {
          "should be a no-op when function reference is not set",
          [](const TestInfo& info) {
            FunctionReference func;

            Call(func);
            Call(info.Env(), func);
          }
      },
      {
          "should call function",
          [](const TestInfo& info) {
            FunctionReference func;
            bool called{ false };
            uint32_t argCount{ 0 };

            func.Reset(Function::New(info.Env(), [&called, &argCount](const Napi::CallbackInfo& info) {
              argCount = info.Length();
              called = true;
            }), 1);

            Call(func);

            Assert::IsTrue(called);
            Assert::Equal(argCount, 0u);

            called = false;
            argCount = 0;

            Call(func, { String::New(info.Env(), "arg") });

            Assert::IsTrue(called);
            Assert::Equal(argCount, 1u);
          }
      },
      {
          "should call function and return value",
          [](const TestInfo& info) {
            auto env{ info.Env() };
            FunctionReference func;
            bool called{ false };
            uint32_t argCount{ 0 };

            func.Reset(Function::New(env, [&called, &argCount](const Napi::CallbackInfo& info) -> Napi::Value {
              argCount = info.Length();
              called = true;

              return String::New(info.Env(), "return-value");
            }), 1);

            auto ret{ Call(env, func) };

            Assert::IsTrue(called);
            Assert::Equal(argCount, 0u);
            Assert::Equal(ret.As<String>().Utf8Value(), std::string("return-value"));

            called = false;
            argCount = 0;

            ret = Call(env, func, { String::New(env, "arg") });

            Assert::IsTrue(called);
            Assert::Equal(argCount, 1u);
            Assert::Equal(ret.As<String>().Utf8Value(), std::string("return-value"));
          }
      }
  };

  spec->Describe("RunScript()")->tests = {
      {
          "should eval script from std::string",
          [](const TestInfo& info) {
            auto result{ RunScript(info.Env(), "(() => 5)()") };

            Assert::IsTrue(result.IsNumber());
            Assert::Equal(result.As<Number>().Int32Value(), 5);
          }
      },
      {
          "should eval script from Napi::String",
          [](const TestInfo& info) {
            auto result{ RunScript(info.Env(), String::New(info.Env(), "(() => 5)()")) };

            Assert::IsTrue(result.IsNumber());
            Assert::Equal(result.As<Number>().Int32Value(), 5);
          }
      },
      {
          "should throw Error from script",
          [](const TestInfo& info) {
            Assert::Throws([env = info.Env()]() { RunScript(env, String()); });
            Assert::Throws([env = info.Env()]() { RunScript(env, "invalid"); });
            Assert::Throws([env = info.Env()]() { RunScript(env, "throw Error('from script')"); });
          }
      }
  };

  spec->Describe("CopyUtf8()")->tests = {
      {
          "should copy string to buffer",
          [](const TestInfo& info) {
            const char* input{ "my_string" };
            static constexpr size_t kBufferSize{ 16 };
            char buffer[kBufferSize];
            auto value{ String::New(info.Env(), input) };

            Assert::CStringEqual(Napi::CopyUtf8(value, buffer, kBufferSize), input);
          }
      },
      {
          "should copy string partial string if buffer is too small",
          [](const TestInfo& info) {
            const char* input{ "123456" };
            static constexpr size_t kBufferSize{ 4 };
            char buffer[kBufferSize];
            auto value{ String::New(info.Env(), input) };

            Assert::CStringEqual(Napi::CopyUtf8(value, buffer, kBufferSize), "123");
          }
      },
      {
          "should return fallback if buffer is null",
          [](const TestInfo& info) {
            const char* input{ "123456" };
            auto value{ String::New(info.Env(), input) };

            Assert::IsNull(Napi::CopyUtf8(value, nullptr, 0));
          }
      },
      {
          "should copy string to internal buffer",
          [](const TestInfo& info) {
            const char* input{ "123456" };
            auto value{ String::New(info.Env(), input) };

            Assert::CStringEqual(Napi::CopyUtf8(value), "123456");
          }
      }
  };

  spec->Describe("StringByteLength()")->tests = {
      {
          "should return the raw byte length of a javascript string",
          [](const TestInfo& info) {
            const char* input{ "123456" };
            auto value{ String::New(info.Env(), input) };

            Assert::Equal(Napi::StringByteLength(value), 6UL);
          }
      },
      {
          "should return 0 if value is not a string",
          [](const TestInfo& info) {
            Assert::Equal(Napi::StringByteLength(Napi::Value()), 0UL);
          }
      }
  };

  spec->Describe("IsNullish()")->tests = {
      {
          "should return true for null value",
          [](const TestInfo& info) {
            Assert::IsTrue(Napi::IsNullish(info.Env(), info.Env().Null()));
          }
      },
      {
          "should return true for undefined value",
          [](const TestInfo& info) {
            Assert::IsTrue(Napi::IsNullish(info.Env(), info.Env().Undefined()));
          }
      },
      {
          "should return false for empty value",
          [](const TestInfo& info) {
            Assert::IsFalse(Napi::IsNullish(info.Env(), Napi::Value()));
          }
      },
      {
          "should return false for string value",
          [](const TestInfo& info) {
            Assert::IsFalse(Napi::IsNullish(info.Env(), Napi::String::New(info.Env(), "test")));
          }
      }
  };
}

} // namespace lse