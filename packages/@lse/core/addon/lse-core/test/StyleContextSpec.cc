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

#include <lse/StyleContext.h>
#include <lse/Style.h>
#include <lse/Image.h>
#include <napi-unit.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

void ComputeObjectFitTest(Style* style, const Image& image, const Rect& bounds, const Rect& expected);

static Style sTestStyle{};

void StyleContextSpec(TestSuite* parent) {
  auto spec{ parent->Describe("StyleContext") };

  spec->afterEach = [](Napi::Env env) {
    sTestStyle = {};
  };

  spec->Describe("ComputeObjectFit()")->tests = {
    {
      "should position image with 'contain'",
      [](const TestInfo&) {
        sTestStyle.SetEnum(StyleProperty::objectFit, "contain");
        ComputeObjectFitTest(
            &sTestStyle,
            Image(200, 200),
            { 0, 0, 40, 40 },
            { 0, 0, 40, 40 });
      }
    },
    {
      "should position image with 'none'",
      [](const TestInfo&) {
        sTestStyle.SetEnum(StyleProperty::objectFit, "none");
        ComputeObjectFitTest(
            &sTestStyle,
            Image(200, 200),
            { 0, 0, 100, 100 },
            { -50, -50, 200, 200 });
      }
    },
    {
      "should position image with 'cover'",
      [](const TestInfo&) {
        sTestStyle.SetEnum(StyleProperty::objectFit, "cover");
        ComputeObjectFitTest(
            &sTestStyle,
            Image(100, 200),
            { 0, 0, 100, 100 },
            { 0, -50, 100, 200 });
      }
    },
    {
      "should position image with 'fill'",
      [](const TestInfo&) {
        sTestStyle.SetEnum(StyleProperty::objectFit, "fill");
        ComputeObjectFitTest(
            &sTestStyle,
            Image(10, 10),
            { 0, 0, 100, 100 },
            { 0, 0, 100, 100 });
      }
    }
  };
}

void ComputeObjectFitTest(Style* style, const Image& image, const Rect& bounds, const Rect& expected) {
  StyleContext context{ 1280, 720, 16 };
  auto fit{ context.ComputeObjectFit(style, bounds, &image) };

  Assert::Equal(fit.x, expected.x);
  Assert::Equal(fit.y, expected.y);
  Assert::Equal(fit.width, expected.width);
  Assert::Equal(fit.height, expected.height);
}

} // namespace lse
