/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <lse/DecodeImage.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

constexpr auto kTestImage = "test/resources/600x400.*";
constexpr auto kTestPngImage = "test/resources/640x480.png";
constexpr auto kTestSvgImage = "test/resources/300x300.svg";
constexpr auto kNotAnImage = "test/resources/test.wav";

void DecodeImageSpec(TestSuite* parent) {
  auto spec{ parent->Describe("DecodeImage") };

  spec->Describe("DecodeImageFromFile()")->tests = {
    {
      "should load a png image from file",
      [](const TestInfo&) {
        auto bytes = DecodeImageFromFile(kTestPngImage, 0, 0);

        Assert::IsNotNull(bytes.Bytes());
        Assert::Equal(bytes.Width(), 640);
        Assert::Equal(bytes.Height(), 480);
        Assert::Equal(bytes.Pitch(), 640 * 4);
      }
    },
    {
      "should load an svg image from file",
      [](const TestInfo&) {
        auto bytes = DecodeImageFromFile(kTestSvgImage, 0, 0);

        Assert::IsNotNull(bytes.Bytes());
        Assert::Equal(bytes.Width(), 300);
        Assert::Equal(bytes.Height(), 300);
        Assert::Equal(bytes.Pitch(), 300 * 4);
      }
    },
    {
      "should load and resize an svg image from file",
      [](const TestInfo&) {
        auto bytes = DecodeImageFromFile(kTestSvgImage, 400, 400);

        Assert::IsNotNull(bytes.Bytes());
        Assert::Equal(bytes.Width(), 400);
        Assert::Equal(bytes.Height(), 400);
        Assert::Equal(bytes.Pitch(), 400 * 4);
      }
    },
    {
      "should load image from file without extension",
      [](const TestInfo&) {
        auto bytes = DecodeImageFromFile(kTestImage, 0, 0);

        Assert::IsNotNull(bytes.Bytes());
        Assert::Equal(bytes.Width(), 600);
        Assert::Equal(bytes.Height(), 400);
        Assert::Equal(bytes.Pitch(), 600 * 4);
      }
    },
    {
      "should throw exception when file not found",
      [](const TestInfo&) {
        Assert::Throws([](){ DecodeImageFromFile("unknown", 0, 0); });
      }
    },
    {
      "should throw exception when file not found (.* extension)",
      [](const TestInfo&) {
        Assert::Throws([](){ DecodeImageFromFile("unknown.*", 0, 0); });
      }
    },
    {
      "should throw exception when file not found (.svg extension)",
      [](const TestInfo&) {
        Assert::Throws([](){ DecodeImageFromFile("unknown.svg", 0, 0); });
      }
    },
    {
      "should throw exception when file is on disk, but not an image",
      [](const TestInfo&) {
        Assert::Throws([](){ DecodeImageFromFile(kNotAnImage, 0, 0); });
      }
    }
  };
}

} // namespace lse
