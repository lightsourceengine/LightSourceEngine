/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <lse/Resources.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

static const char* kImageFile = "image.png";
static Resources resources;

void ResourcesSpec(TestSuite* parent) {
  auto spec{ parent->Describe("Resources") };

  spec->afterEach = [](Napi::Env env) {
    resources = Resources();
  };

  spec->Describe("HasImage()")->tests = {
      {
          "should return true if image exists",
          [](const TestInfo&) {
            resources.AcquireImage(kImageFile);
            Assert::IsTrue(resources.HasImage(kImageFile));
          }
      },
      {
          "should return false if image does not exist",
          [](const TestInfo&) {
            Assert::IsFalse(resources.HasImage(kImageFile));
          },
      }
  };

  spec->Describe("AcquireImage()")->tests = {
      {
          "should return a new image",
          [](const TestInfo&) {
            auto image = resources.AcquireImage(kImageFile);

            Assert::IsNotNull(image);
            Assert::Equal(image->GetTag(), kImageFile);
            Assert::IsTrue(resources.HasImage(kImageFile));
          }
      },
      {
          "should return an existing image",
          [](const TestInfo&) {
            auto image = resources.AcquireImage(kImageFile);

            Assert::Equal(resources.AcquireImage(kImageFile), image);
          },
      }
  };

  spec->Describe("ReleaseResource()")->tests = {
      {
          "should delete image",
          [](const TestInfo&) {
            auto image = resources.AcquireImage(kImageFile);

            Assert::IsTrue(resources.HasImage(kImageFile));

            auto resource = image.get();

            image = nullptr;

            resources.ReleaseResource(resource, true);

            Assert::IsFalse(resources.HasImage(kImageFile));
          }
      },
      {
          "should be a no-op when resource is nullptr",
          [](const TestInfo&) {
            resources.ReleaseResource(nullptr, true);
            resources.ReleaseResource(nullptr, false);
          },
      }
  };

  spec->Describe("Compact()")->tests = {
      {
          "should defer resource delete to Compact()",
          [](const TestInfo&) {
            auto image = resources.AcquireImage(kImageFile);
            auto imageResource = image.get();

            image.reset();

            resources.ReleaseResource(imageResource, false);

            resources.Compact();

            Assert::IsFalse(resources.HasImage(kImageFile));
          }
      }
  };
}

} // namespace lse
