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

#include <lse/Style.h>
#include <napi-unit.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

static Style testStyle{};
static StyleFilterFunction testFilterFunction{StyleFilterFlipV, ColorWhite.value};

static void SetAllProperties(Style* style) noexcept;

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

  spec->Describe("Reset()")->tests = {
    {
      "should clear all property types",
      [](const TestInfo&) {
        SetAllProperties(&testStyle);

        testStyle.Reset();

        for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
          Assert::IsFalse(testStyle.Exists(static_cast<StyleProperty>(i)));
        }
      }
    }
  };

  spec->Describe("SetUndefined()")->tests = {
    {
      "should set all property types to undefined",
      [](const TestInfo&) {
        SetAllProperties(&testStyle);

        testStyle.Reset();

        for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
          auto property{static_cast<StyleProperty>(i)};

          testStyle.SetUndefined(property);
          Assert::IsFalse(testStyle.Exists(property));
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
      "should return true for all set properties",
      [](const TestInfo&) {
        SetAllProperties(&testStyle);

        for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
          Assert::IsTrue(testStyle.Exists(static_cast<StyleProperty>(i)));
        }
      }
    }
  };
}

static void SetAllProperties(Style* style) noexcept {
  for (int32_t i = 0; i < Count<StyleProperty>(); i++) {
    auto property{static_cast<StyleProperty>(i)};

    switch (StylePropertyMetaGetType(property)) {
      case StylePropertyMetaTypeEnum:
        style->SetEnum(property, StylePropertyValueToString(property, 0));
        break;
      case StylePropertyMetaTypeInteger:
        style->SetInteger(property, 1);
        break;
      case StylePropertyMetaTypeString:
        style->SetString(property, "test");
        break;
      case StylePropertyMetaTypeColor:
        style->SetColor(property, ColorWhite.value);
        break;
      case StylePropertyMetaTypeNumber:
        style->SetNumber(property, StyleValue::OfPoint(1));
        break;
      case StylePropertyMetaTypeTransform:
        style->SetTransform({StyleTransformSpec::OfIdentity()});
        break;
      case StylePropertyMetaTypeFilter:
        style->SetFilter({testFilterFunction});
        break;
      default:
        Assert::Fail("Unsupported style meta type.");
        break;
    }
  }
}

} // namespace lse
