/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <ls/Rect.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace ls {

void RectSpec(TestSuite* parent) {
    auto spec{ parent->Describe("Rect") };

    spec->Describe("Intersect()")->tests = {
        {
            "should intersect two rects",
            [](const TestInfo&) {
                auto intersect = Intersect({ 0, 0, 100, 100 }, { 50, 50, 100, 100 });

                Assert::IsFalse(IsEmpty(intersect));
                Assert::Equal(intersect.x, 50);
                Assert::Equal(intersect.y, 50);
                Assert::Equal(intersect.width, 50);
                Assert::Equal(intersect.height, 50);
            }
        },
        {
            "should intersect perfectly overlapping rects",
            [](const TestInfo&) {
                auto intersect = Intersect({ 0, 0, 100, 100 }, { 0, 0, 100, 100 });

                Assert::IsFalse(IsEmpty(intersect));
                Assert::Equal(intersect.x, 0);
                Assert::Equal(intersect.y, 0);
                Assert::Equal(intersect.width, 100);
                Assert::Equal(intersect.height, 100);
            }
        },
        {
            "should return empty when rects do not intersect",
            [](const TestInfo&) {
                auto intersect = Intersect({ 0, 0, 10, 10 }, { 50, 50, 100, 100 });

                Assert::IsTrue(IsEmpty(intersect));
            }
        }
    };
}

} // namespace ls
