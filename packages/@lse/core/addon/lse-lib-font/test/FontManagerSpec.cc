/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>

#include <lse/FontManager.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

class TestFontDriver : public FontDriver {
 public:
  void* LoadFontSource(void* data, size_t dataSize, int32_t index) override {
    return {};
  }

  void DestroyFontSource(FontSource* fontSource) override {
  }
};

constexpr auto kDefaultFamily = "default";
constexpr auto kTestFamily = "test";
static std::unique_ptr<FontDriver> sFontDriver;
static FontManager sFontManager{ nullptr };

// Note: Focus on native specific behavior here, as some test coverage happens in Font.spec.js and FontManager.spec.js

void FontManagerSpec(TestSuite* parent) {
  auto spec{ parent->Describe("FontManager") };

  spec->before = [](const Napi::Env& env) {
    sFontDriver = std::make_unique<TestFontDriver>();
  };

  spec->after = [](const Napi::Env& env) {
    sFontDriver = nullptr;
  };

  spec->beforeEach = [](const Napi::Env& env) {
    sFontManager = FontManager(sFontDriver.get());
  };

  spec->afterEach = [](const Napi::Env& env) {
    sFontManager = FontManager{ nullptr };
  };

  spec->Describe("FindFont()")->tests = {
    {
      "should return font with exact match",
      [](const TestInfo&) {
        auto id = sFontManager.CreateFont(kTestFamily, FontStyleNormal, FontWeightNormal);
        auto found = sFontManager.FindFont(kTestFamily, FontStyleNormal, FontWeightNormal);

        Assert::Equal(sFontManager.GetFont(id), found);
      }
    },
    {
      "should return null when font does not match and no default available",
      [](const TestInfo&) {
        sFontManager.CreateFont(kTestFamily, FontStyleNormal, FontWeightNormal);

        auto found = sFontManager.FindFont("does-not-exist", FontStyleNormal, FontWeightNormal);

        Assert::IsNull(found);
      }
    },
    {
      "should should return default when font requested font family does not exist",
      [](const TestInfo&) {
        auto defaultId = sFontManager.CreateFont(kDefaultFamily, FontStyleNormal, FontWeightNormal);

        sFontManager.SetDefaultFontFamily(kDefaultFamily);
        sFontManager.CreateFont(kTestFamily, FontStyleNormal, FontWeightNormal);

        auto found = sFontManager.FindFont("does-not-exist", FontStyleNormal, FontWeightNormal);

        Assert::Equal(found, sFontManager.GetFont(defaultId));
      }
    }
  };
}

} // namespace lse
