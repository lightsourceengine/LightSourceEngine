/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
      "should ...",
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
      "should ...",
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
