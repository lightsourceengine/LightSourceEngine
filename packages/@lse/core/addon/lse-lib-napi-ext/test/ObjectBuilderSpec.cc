/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <napi-unit.h>
#include <napi-ext.h>
#include <ObjectBuilder.h>

using Napi::Assert;
using Napi::ObjectBuilder;
using Napi::String;
using Napi::TestInfo;
using Napi::TestSuite;
using Napi::Value;

namespace lse {

void ObjectBuilderSpec(TestSuite* parent) {
  auto spec{ parent->Describe("ObjectBuilder") };

  spec->Describe("ToObject()")->tests = {
      {
          "should return object",
          [](const TestInfo& info) {
            auto object = ObjectBuilder(info.Env()).ToObject();
            Assert::IsTrue(object.IsObject());
          }
      }
  };

  spec->Describe("WithValue()")->tests = {
      {
          "should add new integer property",
          [](const TestInfo& info) {
            auto object = ObjectBuilder(info.Env())
                .WithValue("prop", 100)
                .ToObject();

            Assert::IsTrue(object.Get("prop").IsNumber());
          }
      },
      {
          "should add new string property",
          [](const TestInfo& info) {
            auto object = ObjectBuilder(info.Env())
                .WithValue("prop", "string")
                .ToObject();

            Assert::IsTrue(object.Get("prop").IsString());
          }
      },
      {
          "should add new value property",
          [](const TestInfo& info) {
            auto object = ObjectBuilder(info.Env())
                .WithValue("prop", Napi::Array::New(info.Env()))
                .ToObject();

            Assert::IsTrue(object.Get("prop").IsArray());
          }
      }
  };

  spec->Describe("WithProperty()")->tests = {
      {
          "should add new property with native getter and setter",
          [](const TestInfo& info) {
            static int32_t value{1};
            auto object = ObjectBuilder(info.Env())
                .WithProperty(
                    "prop",
                    [](napi_env env, napi_callback_info) -> napi_value {
                      return Napi::Number::New(env, value);
                    },
                    [](napi_env env, napi_callback_info info) -> napi_value {
                      Napi::CallbackInfo ci(env, info);
                      value = ci[0].As<Napi::Number>();
                      return {};
                    })
                .ToObject();

            Assert::Equal(object.Get("prop").As<Napi::Number>().Int32Value(), 1);
            object["prop"] = 3;
            Assert::Equal(value, 3);
          }
      },
  };

  spec->Describe("WithMethod()")->tests = {
      {
          "should add a new function",
          [](const TestInfo& info) {
            auto object = ObjectBuilder(info.Env())
                .WithMethod("func", [](napi_env, napi_callback_info) -> napi_value { return {}; })
                .ToObject();

            Assert::IsTrue(object.Get("func").IsFunction());
          }
      }
  };

  spec->Describe("Freeze()")->tests = {
      {
          "should return frozen object",
          [](const TestInfo& info) {
            auto object = ObjectBuilder(info.Env())
                .WithValue("value", 1)
                .Freeze()
                .ToObject();

            auto isFrozen = Napi::RunScript(info.Env(), "Object.isFrozen")
                .As<Napi::Function>();

            Assert::IsTrue(isFrozen.Call({object}).ToBoolean().Value());
          }
      }
  };
}

} // namespace lse
