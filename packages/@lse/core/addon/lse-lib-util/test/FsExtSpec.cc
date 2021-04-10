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
#include <lse/fs-ext.h>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

static constexpr auto kTestFile{"test/resources/roboto.woff"};
static constexpr size_t kTestFileSize{29040};

void FsExtSpec(TestSuite* parent) {
  const auto spec{parent->Describe("fs-ext")};

  spec->Describe("NewByteArray()")->tests = {
    {
      "should create a byte array with size 3",
      [](const TestInfo&) {
        Assert::Equal(NewByteArray(3).size(), 3u);
      }
    },
    {
      "should create an empty array with size 0",
      [](const TestInfo&) {
        Assert::IsTrue(NewByteArray(0).empty());
      }
    },
    {
      "should create an array from buffer of size 3",
      [](const TestInfo&) {
        uint8_t buffer[3]{};
        Assert::Equal(NewByteArray(buffer, sizeof buffer).size(), 3u);
      }
    },
    {
      "should create ab empty array from null buffer",
      [](const TestInfo&) {
        Assert::IsTrue(NewByteArray(nullptr, 0).empty());
      }
    }
  };

  spec->Describe("ReadFileContents()")->tests = {
    {
      "should read contents of test file",
      [](const TestInfo&) {
        Assert::Equal(ReadFileContents(kTestFile).size(), kTestFileSize);
      }
    },
    {
      "should return empty array for nullptr",
      [](const TestInfo&) {
        Assert::IsTrue(ReadFileContents(nullptr).empty());
      }
    },
    {
      "should return empty array for empty string",
      [](const TestInfo&) {
        Assert::IsTrue(ReadFileContents("").empty());
      }
    },
    {
        "should return empty array for file not found",
      [](const TestInfo&) {
        Assert::IsTrue(ReadFileContents("not-a-real-file").empty());
      }
    }
  };
}

} // namespace lse
