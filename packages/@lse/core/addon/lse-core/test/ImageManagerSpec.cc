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
      "should attach image manager",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
      }
    },
    {
      "should be idempotent",
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
      "should detach image manager",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
        imageManager.Detach();
      }
    },
    {
      "should be idempotent",
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
      "should destroy the image manager",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Destroy();
      }
    },
    {
      "should a detached image manager",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
        imageManager.Detach();
        imageManager.Destroy();
      }
    },
    {
      "should an attached image manager",
      [](const TestInfo&) {
        ImageManager imageManager([](Image*){});
        auto renderer{RefRenderer::New()};

        imageManager.Attach(renderer.get());
        imageManager.Destroy();
      }
    }
  };
}

} // namespace lse
