/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <napi-ext.h>
#include <vector>

using Napi::Assert;
using Napi::NewStringArray;
using Napi::String;
using Napi::TestSuite;

namespace ls {

void NapiExtSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("napi-ext") };
    auto assert{ Assert(env) };

    spec->Describe("NewStringArray")->tests = {
        {
            "should create an Array from a list of strings",
            [env, assert](const Napi::CallbackInfo& info) {
                std::vector<std::string> strings{ "test1", "test2" };
                auto array{ NewStringArray(env, strings) };

                assert.Equal(array.Length(), static_cast<uint32_t>(strings.size()));
                assert.Equal(array.Get(0u).As<String>().Utf8Value(), strings[0]);
                assert.Equal(array.Get(1u).As<String>().Utf8Value(), strings[1]);
            }
        }
    };
}

} // namespace ls
