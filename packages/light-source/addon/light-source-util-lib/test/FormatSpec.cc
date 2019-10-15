/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <ls/Format.h>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

void FormatSpec(Napi::Env env, TestSuite* parent) {
    const auto spec{ parent->Describe("Format") };
    const auto assert{ Assert(env) };

    spec->Describe("Format()")->tests = {
        {
            "should print empty char arrays",
            [assert](const Napi::CallbackInfo& info) {
                const char* nullCharArray = nullptr;
                const char* emptyCharArray = nullptr;

                assert.Equal(Format("x = %s", nullCharArray), std::string("x = null"));
                assert.Equal(Format("x = %s", emptyCharArray), std::string("x = "));
            }
        },
        {
            "should print std::string objects",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(Format("x = %s", std::string("value")), std::string("x = value"));
            }
        },
        {
            "should print nullptr as null",
            [assert](const Napi::CallbackInfo& info) {
                assert.Equal(Format("x = %s", nullptr), std::string("x = null"));
            }
        },
        {
            "should print large string",
            [assert](const Napi::CallbackInfo& info) {
                std::string str(255, 'x');

                assert.Equal(Format("%s", str), str);
            }
        }
    };
}

} // namespace ls
