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

#include <napi-unit.h>

#include <lse/Rect.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

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

} // namespace lse
