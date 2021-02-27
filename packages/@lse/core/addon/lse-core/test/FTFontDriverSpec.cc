/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <fstream>
#include <napi-unit.h>
#include <lse/FTFontDriver.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

// loadable font files
constexpr auto kTestTTF = "test/resources/roboto.ttf";
constexpr auto kTestWOFF = "test/resources/roboto.woff";
// constexpr auto kTestWOFF2 = "test/resources/roboto.woff2";
constexpr auto kTestOTF = "test/resources/roboto.otf";
// invalid font files
constexpr auto kTestEOT = "test/resources/roboto.eot";
constexpr auto kTestSVG = "test/resources/roboto.svg";
constexpr auto kTestNotAFont = "test/resources/test.wav";
constexpr auto kTestFileDoesNotExist = "name-of-the-unknown-file";

static std::unique_ptr<FTFontDriver> sTestFontDriver{};
static FontSource* sTestFace{};

enum class AssertType {
  IS_NULL,
  IS_NOT_NULL
};

static void TestLoadFontSourceFromFile(const char* filename, AssertType assertType);
static void TestLoadFontSourceFromMemory(const char* filename, AssertType assertType);

void FTFontDriverSpec(TestSuite* parent) {
  auto spec{ parent->Describe("FTFontDriver") };

  spec->before = [](const Napi::Env& env) {
    sTestFontDriver = std::make_unique<FTFontDriver>();
  };

  spec->after = [](const Napi::Env& env) {
    sTestFontDriver = nullptr;
  };

  spec->afterEach = [](const Napi::Env& env) {
    sTestFontDriver->DestroyFontSource(sTestFace);
    sTestFace = nullptr;
  };

  spec->Describe("LoadFontSource()")->tests = {
    {
      "should open TTF from file",
      [](const TestInfo&) {
        TestLoadFontSourceFromFile(kTestTTF, AssertType::IS_NOT_NULL);
      }
    },
    {
      "should open TTF from memory",
      [](const TestInfo&) {
        TestLoadFontSourceFromMemory(kTestTTF, AssertType::IS_NOT_NULL);
      }
    },
    {
      "should open WOFF from file",
      [](const TestInfo&) {
        TestLoadFontSourceFromFile(kTestWOFF, AssertType::IS_NOT_NULL);
      }
    },
    {
      "should open WOFF from memory",
      [](const TestInfo&) {
        TestLoadFontSourceFromMemory(kTestWOFF, AssertType::IS_NOT_NULL);
      }
    },
// TODO: re-add when compiling against brotli works on all platforms
//    {
//      "should open WOFF2 from file",
//      [](const TestInfo&) {
//        TestLoadFontSourceFromFile(kTestWOFF2, AssertType::IS_NOT_NULL);
//      }
//    },
//    {
//      "should open WOFF2 from memory",
//      [](const TestInfo&) {
//        TestLoadFontSourceFromMemory(kTestWOFF2, AssertType::IS_NOT_NULL);
//      }
//    },
    {
      "should open OTF from file",
      [](const TestInfo&) {
        TestLoadFontSourceFromFile(kTestOTF, AssertType::IS_NOT_NULL);
      }
    },
    {
      "should open OTF from memory",
      [](const TestInfo&) {
        TestLoadFontSourceFromMemory(kTestOTF, AssertType::IS_NOT_NULL);
      }
    },
    {
      "should fail to open SVG font from file",
      [](const TestInfo&) {
        TestLoadFontSourceFromFile(kTestSVG, AssertType::IS_NULL);
      }
    },
    {
      "should fail to open SVG font from memory",
      [](const TestInfo&) {
        TestLoadFontSourceFromMemory(kTestSVG, AssertType::IS_NULL);
      }
    },
    {
      "should fail to open EOT font from file",
      [](const TestInfo&) {
        TestLoadFontSourceFromFile(kTestEOT, AssertType::IS_NULL);
      }
    },
    {
      "should fail to open EOT font from memory",
      [](const TestInfo&) {
        TestLoadFontSourceFromMemory(kTestEOT, AssertType::IS_NULL);
      }
    },
    {
      "should fail to open non-font from file",
      [](const TestInfo&) {
        TestLoadFontSourceFromFile(kTestNotAFont, AssertType::IS_NULL);
      }
    },
    {
      "should fail to open non-font from memory",
      [](const TestInfo&) {
        TestLoadFontSourceFromMemory(kTestNotAFont, AssertType::IS_NULL);
      }
    },
    {
      "should return null for null filename",
      [](const TestInfo&) {
        Assert::IsNull(sTestFontDriver->LoadFontSource(nullptr, 0));
      }
    },
    {
      "should return null for empty filename",
      [](const TestInfo&) {
        Assert::IsNull(sTestFontDriver->LoadFontSource("", 0));
      }
    },
    {
      "should return null for empty data",
      [](const TestInfo&) {
        Assert::IsNull(sTestFontDriver->LoadFontSource(nullptr, 0, 0));
      }
    },
    {
      "should return null for file not found",
      [](const TestInfo&) {
        Assert::IsNull(sTestFontDriver->LoadFontSource(kTestFileDoesNotExist, 0));
      }
    }
  };

  spec->Describe("DestroyFontSource()")->tests = {
    {
      "should do nothing when font source is null",
      [](const TestInfo&) {
        sTestFontDriver->DestroyFontSource(nullptr);
      }
    },
  };
}

static void ExpectFace(FontSource* face, AssertType assertType) noexcept {
  switch (assertType) {
    case AssertType::IS_NULL:
      Assert::IsNull(face);
      break;
    case AssertType::IS_NOT_NULL:
      Assert::IsNotNull(face);
      break;
    default:
      Assert::Fail("unsupported assert type");
      break;
  }
}

static void TestLoadFontSourceFromFile(const char* filename, AssertType assertType) {
  sTestFace = sTestFontDriver->LoadFontSource(filename, 0);
  ExpectFace(sTestFace, assertType);
}

static void TestLoadFontSourceFromMemory(const char* filename, AssertType assertType) {
  std::ifstream testFile(filename, std::ios::binary);
  std::vector<char> fileContents;

  fileContents.assign(std::istreambuf_iterator<char>(testFile), std::istreambuf_iterator<char>());

  sTestFace = sTestFontDriver->LoadFontSource(fileContents.data(), fileContents.size(), 0);
  ExpectFace(sTestFace, assertType);
}

} // namespace lse
