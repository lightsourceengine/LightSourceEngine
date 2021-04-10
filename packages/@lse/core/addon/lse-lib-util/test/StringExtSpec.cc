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
#include <lse/string-ext.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

void StringExtSpec(TestSuite* parent) {
  const auto spec{ parent->Describe("StringExt") };

  spec->Describe("ToLowercase()")->tests = {
    {
      "should convert std::string to lowercase",
      [](const TestInfo&) {
        std::string str = "TEST";

        Assert::Equal(ToLowercase(str), "test");
      }
    },
    {
      "should convert cstring to lowercase",
      [](const TestInfo&) {
        char str[] = "TEST";

        Assert::IsTrue(strcmp(ToLowercase(str), "test") == 0);
      }
    },
    {
      "should return nullptr for nullptr argument",
      [](const TestInfo&) {
        Assert::IsNull(ToLowercase(nullptr));
      }
    }
  };

  spec->Describe("EqualsIgnoreCase()")->tests = {
    {
      "should return true when strings match",
      [](const TestInfo&) {
        Assert::IsTrue(EqualsIgnoreCase("test", "test"));
        Assert::IsTrue(EqualsIgnoreCase("test", "TEST"));
        Assert::IsTrue(EqualsIgnoreCase("TEST", "TEST"));
        Assert::IsTrue(EqualsIgnoreCase("TEST", "test"));
      }
    },
    {
      "should true for empty string comparison",
      [](const TestInfo&) {
        Assert::IsTrue(EqualsIgnoreCase("", ""));
        Assert::IsTrue(EqualsIgnoreCase("", nullptr));
        Assert::IsTrue(EqualsIgnoreCase(std::string(), ""));
        Assert::IsTrue(EqualsIgnoreCase(std::string(), nullptr));
      }
    },
    {
      "should return false when strings don't match",
      [](const TestInfo&) {
        Assert::IsFalse(EqualsIgnoreCase("test", ""));
        Assert::IsFalse(EqualsIgnoreCase("test", nullptr));
        Assert::IsFalse(EqualsIgnoreCase("", "test"));
        Assert::IsFalse(EqualsIgnoreCase(std::string(), "test"));
        Assert::IsFalse(EqualsIgnoreCase("xxxxxxxxxxxx", "shorter"));
        Assert::IsFalse(EqualsIgnoreCase("x", "longer"));
      }
    }
  };

  spec->Describe("EndsWith()")->tests = {
    {
      "should return true if string ends with .ext",
      [](const TestInfo&) {
        Assert::IsTrue(EndsWith("test.ext", ".ext"));
      }
    },
    {
      "should return false if string does not end with .ext",
      [](const TestInfo&) {
        Assert::IsFalse(EndsWith("", ".ext"));
        Assert::IsFalse(EndsWith("test", ".ext"));
      }
    },
    {
      "should return false if either arg is null",
      [](const TestInfo&) {
        Assert::IsFalse(EndsWith(nullptr, nullptr));
        Assert::IsFalse(EndsWith("test", nullptr));
        Assert::IsFalse(EndsWith(nullptr, "test"));
      }
    }
  };

  spec->Describe("StartsWith()")->tests = {
    {
      "should return true if string starts with pro",
      [](const TestInfo&) {
        Assert::IsTrue(StartsWith("profile", "pro"));
      }
    },
    {
      "should return false if string does not start with pro",
      [](const TestInfo&) {
        Assert::IsFalse(StartsWith("", "pro"));
        Assert::IsFalse(StartsWith("test", "pro"));
      }
    },
    {
      "should return false if either arg is null",
      [](const TestInfo&) {
        Assert::IsFalse(StartsWith(nullptr, nullptr));
        Assert::IsFalse(StartsWith("test", nullptr));
        Assert::IsFalse(StartsWith(nullptr, "test"));
      }
    }
  };
}

} // namespace lse
