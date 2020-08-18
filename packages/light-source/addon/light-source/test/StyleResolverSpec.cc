/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/Resources.h>
#include <ls/StyleResolver.h>
#include <ls/Style.h>
#include <napi-unit.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace ls {

void ResolveObjectFitTest(Style* style, const Image& image, const Rect& bounds, const Rect& expected);

static Style* sTestStyle{nullptr};

void StyleResolverSpec(TestSuite* parent) {
    auto spec{ parent->Describe("StyleResolver") };

    spec->beforeEach = [](Napi::Env env) {
        sTestStyle = Style::New(env);
    };

    spec->afterEach = [](Napi::Env env) {
        if (sTestStyle) {
            sTestStyle->Unref();
            sTestStyle = nullptr;
        }
    };

    spec->Describe("ResolveObjectFit()")->tests = {
        {
            "should position image with 'contain'",
            [](const TestInfo&) {
                sTestStyle->Set_objectFit(StyleObjectFitContain);
                ResolveObjectFitTest(
                    sTestStyle,
                    Image::Mock("test", 200, 200),
                    { 0, 0, 40, 40 },
                    { 0, 0, 40, 40 });
            }
        },
        {
            "should position image with 'none'",
            [](const TestInfo&) {
                sTestStyle->Set_objectFit(StyleObjectFitNone);
                ResolveObjectFitTest(
                    sTestStyle,
                    Image::Mock("test", 200, 200),
                    { 0, 0, 100, 100 },
                    { -50, -50, 200, 200 });
            }
        },
        {
            "should position image with 'cover'",
            [](const TestInfo&) {
                sTestStyle->Set_objectFit(StyleObjectFitCover);
                ResolveObjectFitTest(
                    sTestStyle,
                    Image::Mock("test", 100, 200),
                    { 0, 0, 100, 100 },
                    { 0, -50, 100, 200 });
            }
        },
        {
            "should position image with 'fill'",
            [](const TestInfo&) {
                sTestStyle->Set_objectFit(StyleObjectFitFill);
                ResolveObjectFitTest(
                    sTestStyle,
                    Image::Mock("test", 10, 10),
                    { 0, 0, 100, 100 },
                    { 0, 0, 100, 100 });
            }
        }
    };
}

void ResolveObjectFitTest(Style* style, const Image& image, const Rect& bounds, const Rect& expected) {
    StyleResolver resolver{ 1280, 720, 16 };
    auto fit{ resolver.ResolveObjectFit(style, bounds, &image) };

    Assert::Equal(fit.x, expected.x);
    Assert::Equal(fit.y, expected.y);
    Assert::Equal(fit.width, expected.width);
    Assert::Equal(fit.height, expected.height);
}

} // namespace ls