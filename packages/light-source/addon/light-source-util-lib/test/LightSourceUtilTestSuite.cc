/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>

using Napi::Env;
using Napi::HandleScope;
using Napi::Object;

namespace ls {
    void FileSystemSpec(Napi::Env env, Napi::TestSuite* parent);
    void FormatSpec(Napi::Env env, Napi::TestSuite* parent);
}

Object Init(Env env, Object exports) {
    HandleScope scope(env);

    exports["test"] = Napi::TestSuite::Build(env, "light-source-util-lib native tests", {
        &ls::FileSystemSpec,
        &ls::FormatSpec,
    });

    return exports;
}

NODE_API_MODULE(LightSourceUtilTestSuite, Init);
