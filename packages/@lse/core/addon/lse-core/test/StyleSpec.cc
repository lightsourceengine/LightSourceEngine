/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/Style.h>
#include <napi-unit.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

static Style testStyle;

void StyleSpec(TestSuite* parent) {
    auto spec{ parent->Describe("Style") };

    spec->afterEach = [](Napi::Env env) {
        testStyle = {};
    };

    spec->Describe("Or()")->tests = {
        {
            "should return an instance to an empty style if arg is null",
            [](const TestInfo&) {
                Assert::IsNotNull(Style::Or(nullptr));
            }
        },
        {
            "should return the arg when the arg is non-null",
            [](const TestInfo&) {
                Assert::Equal(Style::Or(&testStyle), &testStyle);
            }
        }
    };

    spec->Describe("IsEmpty()")->tests = {
        {
            "should return true for all properties on a new style object",
            [](const TestInfo&) {
                for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
                    Assert::IsTrue(testStyle.IsEmpty(static_cast<StyleProperty>(i)));
                }
            }
        }
    };

    spec->Describe("Exists()")->tests = {
        {
            "should return false for all properties on a new style object",
            [](const TestInfo&) {
                for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
                    Assert::IsFalse(testStyle.Exists(static_cast<StyleProperty>(i)));
                }
            }
        },
        {
            "should return true if color property exists",
            [](const TestInfo&) {
                auto property = StyleProperty::backgroundColor;

                testStyle.SetColor(property, 0xFFFF0000);

                Assert::IsTrue(testStyle.Exists(property));
            }
        },
        {
            "should return true if integer property exists",
            [](const TestInfo&) {
                auto property = StyleProperty::maxLines;

                testStyle.SetInteger(property, 1);

                Assert::IsTrue(testStyle.Exists(property));
            }
        },
        {
            "should return true if string property exists",
            [](const TestInfo&) {
                auto property = StyleProperty::fontFamily;

                testStyle.SetString(property, "test");

                Assert::IsTrue(testStyle.Exists(property));
            }
        },
        {
            "should return true if number property exists",
            [](const TestInfo&) {
                auto property = StyleProperty::width;

                testStyle.SetNumber(property, StyleValue::OfPoint(3.f));

                Assert::IsTrue(testStyle.Exists(property));
            }
        },
        {
            "should return true if transform property exists",
            [](const TestInfo&) {
                auto property = StyleProperty::transform;

                testStyle.SetTransform({ StyleTransformSpec::OfIdentity() });

                Assert::IsTrue(testStyle.Exists(property));
            }
        }
    };
}

} // namespace lse
