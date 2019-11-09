/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <ls/StyleUtils.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace ls {

struct MockImage {
    float width{};
    float height{};
    bool hasCapInsets{false};

    float GetWidthF() const { return this->width; }
    float GetHeightF() const { return this->height; }
    float GetAspectRatio() const { return this->height != 0 ? this->width / this->height : 0.f; }
    bool HasCapInsets() const { return this->hasCapInsets; }
};

struct MockScene {
    float width{};
    float height{};
    int32_t rootFontSize{16};

    float GetWidth() const { return this->width; }
    float GetHeight() const { return this->height; }
    float GetViewportMin() const { return this->height > this->width ? this->width : this->height; }
    float GetViewportMax() const { return this->height > this->width ? this->height : this->width; }
    int32_t GetRootFontSize() const { return this->rootFontSize; }
};

void ComputeObjectFitRectTest(StyleObjectFit objectFit, const MockImage& image, const Rect& bounds,
    const Rect& expected);

void StyleUtilsSpec(TestSuite* parent) {
    auto spec{ parent->Describe("StyleUtils") };

    spec->Describe("CreateRoundedRectangleUri()")->tests = {
        {
            "should create rounded rectangle svg xml",
            [](const TestInfo&) {
                auto svg{ CreateRoundedRectangleUri(10, 10, 10, 10, 4) };

                // TODO: validate svg string
                Assert::IsFalse(svg.empty());
            }
        },
    };

    spec->Describe("ComputeObjectFitRect()")->tests = {
        {
            "should position image with 'contain'",
            [](const TestInfo&) {
                ComputeObjectFitRectTest(
                    StyleObjectFitContain,
                    { 200, 200 },
                    { 0, 0, 40, 40 },
                    { 0, 0, 40, 40 });
            }
        },
        {
            "should position image with 'none'",
            [](const TestInfo&) {
                ComputeObjectFitRectTest(
                    StyleObjectFitNone,
                    { 200, 200 },
                    { 0, 0, 100, 100 },
                    { -50, -50, 200, 200 });
            }
        },
        {
            "should position image with 'cover'",
            [](const TestInfo&) {
                ComputeObjectFitRectTest(
                    StyleObjectFitCover,
                    { 100, 200 },
                    { 0, 0, 100, 100 },
                    { 0, -50, 100, 200 });
            }
        },
        {
            "should position image with 'fill'",
            [](const TestInfo&) {
                ComputeObjectFitRectTest(
                    StyleObjectFitFill,
                    { 10, 10 },
                    { 0, 0, 100, 100 },
                    { 0, 0, 100, 100 });
            }
        },
        {
            "should always position capinsets image with 'fill'",
            [](const TestInfo&) {
                for (int32_t i = 0; i < Count<StyleObjectFit>(); i++) {
                    ComputeObjectFitRectTest(
                        static_cast<StyleObjectFit>(i),
                        { 10, 10, true },
                        { 0, 0, 100, 100 },
                        { 0, 0, 100, 100 });
                }
            }
        }
    };

    // TODO: implement tests
//    spec->Describe("ComputeIntegerPointValue()")->tests = {
//        {
//            "should ...",
//            [=](const TestInfo&) {
//            }
//        },
//    };
//
//    spec->Describe("ComputeLineHeight()")->tests = {
//        {
//            "should ...",
//            [=](const TestInfo&) {
//            }
//        },
//    };
}

void ComputeObjectFitRectTest(StyleObjectFit objectFit, const MockImage& image, const Rect& bounds,
        const Rect& expected) {
    MockScene scene{ 1280, 720, 16 };
    Rect fit = ComputeObjectFitRect(objectFit, {}, {}, bounds, &image, &scene);

    Assert::Equal(fit.x, expected.x);
    Assert::Equal(fit.y, expected.y);
    Assert::Equal(fit.width, expected.width);
    Assert::Equal(fit.height, expected.height);
}

} // namespace ls
