/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
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
}

} // namespace lse
