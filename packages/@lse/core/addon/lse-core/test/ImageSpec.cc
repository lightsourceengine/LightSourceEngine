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
#include <lse/Image.h>
#include <lse/RefRenderer.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

constexpr auto kTestPngImage = "test/resources/640x480.png";

void ImageSpec(TestSuite* parent) {
  auto spec{ parent->Describe("Image") };

  spec->Describe("constructor")->tests = {
    {
      "should create a new Image object",
      [](const TestInfo&) {
        ImageRequest request{ kTestPngImage, 10, 10};
        Image image(request);

        Assert::IsTrue(image.GetState() == ImageState::Init);

        Assert::IsTrue(image.GetRequest() == request);
        Assert::Equal(image.Width(), 0);
        Assert::Equal(image.Height(), 0);
        Assert::IsNull(image.GetTexture());
        Assert::IsFalse(image.IsAttached());
      }
    }
  };

  spec->Describe("Attach()")->tests = {
    {
      "should attach image to a renderer",
      [](const TestInfo&) {
        Image image({ kTestPngImage, 10, 10});
        auto renderer = RefRenderer::New();

        Assert::IsFalse(image.IsAttached());
        image.Attach(renderer.get());
        Assert::IsTrue(image.IsAttached());
      }
    }
  };

  spec->Describe("Detach()")->tests = {
    {
      "should detach an image from the renderer",
      [](const TestInfo&) {
        Image image({ kTestPngImage, 10, 10});
        auto renderer = RefRenderer::New();

        Assert::IsFalse(image.IsAttached());
        image.Attach(renderer.get());
        Assert::IsTrue(image.IsAttached());
        image.Detach(renderer.get());
        Assert::IsFalse(image.IsAttached());
      }
    }
  };
}

} // namespace lse
