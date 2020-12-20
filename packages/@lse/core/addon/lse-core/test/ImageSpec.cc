/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <ls/Resources.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace ls {

void ImageSpec(TestSuite* parent) {
    auto spec{ parent->Describe("Image") };

    spec->Describe("Image()")->tests = {
        {
            "should create a new Image object",
            [](const TestInfo&) {
                Image image("image.jpg");

                Assert::Equal(image.Width(), 0);
                Assert::Equal(image.Height(), 0);
                Assert::Equal(image.GetState(), Resource::State::Init);
            }
        }
    };
}

} // namespace ls
