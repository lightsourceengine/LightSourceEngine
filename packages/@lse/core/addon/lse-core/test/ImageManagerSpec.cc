/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <lse/ImageManager.h>
#include <lse/RefRenderer.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

void ImageManagerSpec(TestSuite* parent) {
  auto spec{ parent->Describe("ImageManager") };

  spec->Describe("Attach()")->tests = {
    {
      "should ...",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
      }
    },
    {
      "should ...",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
        imageManager.Attach(renderer.get());
      }
    }
  };

  spec->Describe("Detach()")->tests = {
    {
      "should ...",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
        imageManager.Detach();
      }
    },
    {
      "should ...",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
        imageManager.Detach();
        imageManager.Detach();
      }
    }
  };

  spec->Describe("Destroy()")->tests = {
    {
      "should ...",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Destroy();
      }
    },
    {
      "should ...",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
        imageManager.Detach();
        imageManager.Destroy();
      }
    }
  };
}

} // namespace lse
