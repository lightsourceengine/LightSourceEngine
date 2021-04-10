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

#include <lse/Font.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

static auto sTestFamily{"test"};
static Font sFont{ sTestFamily, FontStyleNormal, FontWeightNormal };
static int32_t sTemp{};
static void* sTestListener{&sTemp};
auto sTestFontSource{ reinterpret_cast<FontSource*>(1) };
static auto sCalled{false};

// Note: Focus on native specific behavior here, as some test coverage happens in Font.spec.js and FontManager.spec.js

void FontSpec(TestSuite* parent) {
  auto spec{ parent->Describe("Font") };

  spec->beforeEach = [](const Napi::Env& env) {
    sFont = { sTestFamily, FontStyleNormal, FontWeightNormal };
    sCalled = false;
  };

  spec->Describe("constructor")->tests = {
    {
      "should create font in init state",
      [](const TestInfo& ti) {
        Assert::CStringEqual(sFont.GetFamily().c_str(), sTestFamily);
        Assert::Equal(sFont.GetStyle(), FontStyleNormal);
        Assert::Equal(sFont.GetWeight(), FontWeightNormal);
        Assert::Equal(sFont.GetFontStatus(), FontStatusInit);
        Assert::IsNull(sFont.GetFontSource());
      }
    }
  };

  spec->Describe("SetFontSource()")->tests = {
    {
        "should update status and source",
      [](const TestInfo& ti) {
        sFont.SetFontSource(sTestFontSource);

        Assert::Equal(sFont.GetFontStatus(), FontStatusReady);
        Assert::Equal(sFont.GetFontSource(), sTestFontSource);
      }
    },
    {
        "should notify listeners of status change",
      [](const TestInfo& ti) {
        sFont.AddListener(sTestListener, [](Font* font, void* listener, FontStatus status) {
          Assert::Equal(font, &sFont);
          Assert::Equal(listener, sTestListener);
          Assert::Equal(status, FontStatusError);
          sCalled = true;
        });

        sFont.SetFontSource(nullptr);

        Assert::IsTrue(sCalled);
      }
    },
    {
      "should update normally with null listener",
      [](const TestInfo& ti) {
        sFont.AddListener(nullptr, [](Font* font, void* listener, FontStatus status) {
          sCalled = true;
        });

        sFont.SetFontSource(sTestFontSource);

        Assert::IsFalse(sCalled);
        Assert::Equal(sFont.GetFontStatus(), FontStatusReady);
        Assert::Equal(sFont.GetFontSource(), sTestFontSource);
      }
    },
    {
      "should update normally with null callback",
      [](const TestInfo& ti) {
        sFont.AddListener(sTestListener, nullptr);

        sFont.SetFontSource(sTestFontSource);

        Assert::Equal(sFont.GetFontStatus(), FontStatusReady);
        Assert::Equal(sFont.GetFontSource(), sTestFontSource);
      }
    },
    {
      "should update normally when listener is added and removed",
      [](const TestInfo& ti) {
        sFont.AddListener(sTestListener, [](Font* font, void* listener, FontStatus status) {
          sCalled = true;
        });
        sFont.RemoveListener(sTestListener);

        sFont.SetFontSource(sTestFontSource);

        Assert::IsFalse(sCalled);
      }
    }
  };

  spec->Describe("RemoveListener()")->tests = {
    {
      "should tolerate nullptr as listener arg",
      [](const TestInfo& ti) {
        sFont.RemoveListener(nullptr);
      }
    }
  };

  spec->Describe("AddListener()")->tests = {
    {
      "should tolerate nullptr as listener arg",
      [](const TestInfo& ti) {
        sFont.AddListener(nullptr, [](Font*, void*, FontStatus) {});
      }
    },
    {
      "should tolerate nullptr as listener arg",
      [](const TestInfo& ti) {
        sFont.AddListener(sTestListener, nullptr);
      }
    }
  };
}

} // namespace lse
