/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/Resources.h>
#include <lse/StyleContext.h>
#include <lse/Style.h>
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
                    Image::Mock("test", 200, 200),
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
                    Image::Mock("test", 200, 200),
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
                    Image::Mock("test", 100, 200),
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
                    Image::Mock("test", 10, 10),
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
