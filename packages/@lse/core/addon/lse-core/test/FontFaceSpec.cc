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

void FontFaceSpec(TestSuite* parent) {
  auto spec{ parent->Describe("FontFace") };

  spec->Describe("FontFace()")->tests = {
      {
          "should use filename as font name",
          [](const TestInfo&) {
            FontFace face("file:font.ttf");

            Assert::Equal(face.GetFamily(), "font");
            Assert::Equal(face.GetStyle(), StyleFontStyleNormal);
            Assert::Equal(face.GetWeight(), StyleFontWeightNormal);
          }
      },
      {
          "should use family name, style and weight from uri parameters",
          [](const TestInfo&) {
            FontFace face("file:font.ttf?family=test&style=italic&weight=bold");

            Assert::Equal(face.GetFamily(), "test");
            Assert::Equal(face.GetStyle(), StyleFontStyleItalic);
            Assert::Equal(face.GetWeight(), StyleFontWeightBold);
          }
      },
      {
          "should use ignore invalid style and weight query parameters",
          [](const TestInfo&) {
            FontFace face("file:font.ttf?family=test&style=xxx&weight=xxx");

            Assert::Equal(face.GetFamily(), "test");
            Assert::Equal(face.GetStyle(), StyleFontStyleNormal);
            Assert::Equal(face.GetWeight(), StyleFontWeightNormal);
          }
      }
  };

  spec->Describe("Equals()")->tests = {
      { "should equal same font spec",
        [](const TestInfo&) {
          auto face = std::make_shared<FontFace>("file:font.ttf");

          Assert::IsTrue(FontFace::Equals(face, "font", StyleFontStyleNormal, StyleFontWeightNormal));
        }
      },
      { "should not equal font spec",
        [](const TestInfo&) {
          auto face = std::make_shared<FontFace>("file:font.ttf");

          Assert::IsFalse(FontFace::Equals(face, "xxx", StyleFontStyleNormal, StyleFontWeightNormal));
        }
      },
  };
}

} // namespace lse
