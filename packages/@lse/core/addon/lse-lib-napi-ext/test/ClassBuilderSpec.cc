/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <napi-unit.h>
#include <napi-ext.h>

using Napi::Assert;
using Napi::ClassBuilder;
using Napi::String;
using Napi::TestInfo;
using Napi::TestSuite;
using Napi::Value;

namespace ls {

void ClassBuilderSpec(TestSuite* parent) {
    auto spec{ parent->Describe("ClassBuilder") };

    spec->Describe("ToConstructor()")->tests = {
        {
            "should return constructor",
            [](const TestInfo& info) {
                auto constructor = ClassBuilder(info.Env(), "T")
                    .ToConstructor();

                Assert::IsTrue(constructor.IsFunction());
            }
        }
    };

    spec->Describe("ToConstructorReference()")->tests = {
        {
            "should return weak reference",
            [](const TestInfo& info) {
                auto constructor = ClassBuilder(info.Env(), "T")
                    .ToConstructorReference(false);

                Assert::IsFalse(constructor.IsEmpty());
                Assert::IsTrue(constructor.Value().IsFunction());
            }
        },
        {
            "should return permanent reference",
            [](const TestInfo& info) {
                auto constructor = ClassBuilder(info.Env(), "T")
                    .ToConstructorReference(true);

                Assert::IsFalse(constructor.IsEmpty());
                Assert::IsTrue(constructor.Value().IsFunction());
            }
        }
    };

    spec->Describe("WithStaticMethod()")->tests = {
        {
            "should set static method with value return",
            [](const TestInfo& info) {
              auto constructor = ClassBuilder(info.Env(), "T")
                  .WithStaticMethod("m", [](const Napi::CallbackInfo& info) { return Value(); })
                  .ToConstructor();

              Assert::IsTrue(constructor.IsFunction());
              Assert::IsTrue(constructor.Has("m"));
              Assert::IsTrue(constructor.Get("m").IsFunction());
            }
        },
        {
            "should set static method with void return",
            [](const TestInfo& info) {
              auto constructor = ClassBuilder(info.Env(), "T")
                  .WithStaticMethod("v", [](const Napi::CallbackInfo& info) {})
                  .ToConstructor();

              Assert::IsTrue(constructor.IsFunction());
              Assert::IsTrue(constructor.Has("v"));
              Assert::IsTrue(constructor.Get("v").IsFunction());
            }
        }
    };

    spec->Describe("WithStaticValue()")->tests = {
        {
            "should set value using int",
            [](const TestInfo& info) {
              auto constructor = ClassBuilder(info.Env(), "T")
                  .WithStaticValue("x", 3)
                  .ToConstructor();

              Assert::IsTrue(constructor.IsFunction());
              Assert::IsTrue(constructor.Has("x"));
              Assert::IsTrue(constructor.Get("x").IsNumber());
            }
        },
        {
            "should set value",
            [](const TestInfo& info) {
              auto constructor = ClassBuilder(info.Env(), "T")
                  .WithStaticValue("x", String::New(info.Env(), "test"))
                  .ToConstructor();

              Assert::IsTrue(constructor.IsFunction());
              Assert::IsTrue(constructor.Has("x"));
              Assert::IsTrue(constructor.Get("x").IsString());
            }
        }
    };
}

} // namespace ls
