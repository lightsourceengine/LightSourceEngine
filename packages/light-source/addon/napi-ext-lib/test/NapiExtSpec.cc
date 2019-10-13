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
using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::NewStringArray;
using Napi::String;
using Napi::TestSuite;

namespace ls {

void NapiExtSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("napi-ext") };
    auto assert{ Assert(env) };

    spec->Describe("NewStringArray()")->tests = {
        {
            "should create an Array from a list of strings",
            [env, assert](const CallbackInfo& info) {
                std::vector<std::string> strings{ "test1", "test2" };
                auto array{ NewStringArray(env, strings) };

                assert.Equal(array.Length(), static_cast<uint32_t>(strings.size()));
                assert.Equal(array.Get(0u).As<String>().Utf8Value(), strings[0]);
                assert.Equal(array.Get(1u).As<String>().Utf8Value(), strings[1]);
            }
        }
    };

    spec->Describe("ToUpperCase()")->tests = {
        {
            "should convert string to uppercase",
            [env, assert](const CallbackInfo& info) {
                auto result{ ToUpperCase(String::New(env, "test")) };

                assert.Equal(result, std::string("TEST"));
            }
        },
        {
            "should handle empty string",
            [assert](const CallbackInfo& info) {
                auto result{ ToUpperCase(String()) };

                assert.Equal(result, std::string(""));
            }
        }
    };

    spec->Describe("ToLowerCase()")->tests = {
        {
            "should convert string to uppercase",
            [env, assert](const CallbackInfo& info) {
                auto result{ ToLowerCase(String::New(env, "TEST")) };

                assert.Equal(result, std::string("test"));
            }
        },
        {
            "should handle empty string",
            [assert](const CallbackInfo& info) {
                auto result{ ToLowerCase(String()) };

                assert.Equal(result, std::string(""));
            }
        }
    };

    spec->Describe("Call()")->tests = {
        {
            "should be a no-op when function reference is not set",
            [env](const CallbackInfo& info) {
                FunctionReference func;

                Call(func);
                Call(env, func);
            }
        },
        {
            "should call function",
            [env, assert](const CallbackInfo& info) {
                FunctionReference func;
                bool called{ false };
                uint32_t argCount{ 0 };

                func.Reset(Function::New(env, [&called, &argCount](const CallbackInfo& info){
                    argCount = info.Length();
                    called = true;
                }), 1);

                Call(func);

                assert.IsTrue(called);
                assert.Equal(argCount, 0u);

                called = false;
                argCount = 0;

                Call(func, { String::New(env, "arg") });

                assert.IsTrue(called);
                assert.Equal(argCount, 1u);
            }
        },
        {
            "should call function and return value",
            [env, assert](const CallbackInfo& info) {
                FunctionReference func;
                bool called{ false };
                uint32_t argCount{ 0 };

                func.Reset(Function::New(env, [&called, &argCount](const CallbackInfo& info) -> Napi::Value {
                    argCount = info.Length();
                    called = true;

                    return String::New(info.Env(), "return-value");
                }), 1);

                auto ret{ Call(env, func) };

                assert.IsTrue(called);
                assert.Equal(argCount, 0u);
                assert.Equal(ret.As<String>().Utf8Value(), std::string("return-value"));

                called = false;
                argCount = 0;

                ret = Call(env, func, { String::New(env, "arg") });

                assert.IsTrue(called);
                assert.Equal(argCount, 1u);
                assert.Equal(ret.As<String>().Utf8Value(), std::string("return-value"));
            }
        }
    };
}

} // namespace ls
