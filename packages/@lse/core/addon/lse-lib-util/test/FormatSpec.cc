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

void FormatSpec(TestSuite* parent) {
  const auto spec{ parent->Describe("Format") };

  spec->Describe("Format()")->tests = {
      {
          "should print empty char arrays",
          [](const TestInfo&) {
            const char* nullCharArray = nullptr;
            const char* emptyCharArray = "";

            Assert::Equal(Format("x = %s", nullCharArray), std::string("x = null"));
            Assert::Equal(Format("x = %s", emptyCharArray), std::string("x = "));
          }
      },
      {
          "should print std::string objects",
          [](const TestInfo&) {
            Assert::Equal(Format("x = %s", std::string("value")), std::string("x = value"));
          }
      },
      {
          "should print nullptr as null",
          [](const TestInfo&) {
            Assert::Equal(Format("x = %s", nullptr), std::string("x = null"));
          }
      },
      {
          "should print large string",
          [](const TestInfo&) {
            std::string str(255, 'x');

            Assert::Equal(Format("%s", str), str);
          }
      }
  };
}

} // namespace lse
