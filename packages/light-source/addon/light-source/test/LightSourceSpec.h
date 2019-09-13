/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#ifdef LIGHT_SOURCE_NATIVE_TESTS

#include <napi-unit.h>

namespace ls {

void FileSystemSpec(Napi::Env env, Napi::TestSuite* parent);
void SurfaceSpec(Napi::Env env, Napi::TestSuite* parent);

inline
Napi::Value LightSourceSpec(Napi::Env env) {
    return Napi::TestSuite::Build(env, "light-source native tests", {
        &FileSystemSpec,
        &SurfaceSpec
    });
}

} // namespace ls

#endif // LIGHT_SOURCE_ENABLE_NATIVE_TESTS
