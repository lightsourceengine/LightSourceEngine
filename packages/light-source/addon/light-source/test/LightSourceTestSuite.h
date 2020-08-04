/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-unit.h>

namespace ls {

void ExecutorSpec(Napi::TestSuite* parent);
void StyleUtilsSpec(Napi::TestSuite* parent);

inline
Napi::Value LightSourceTestSuite(Napi::Env env) {
    return Napi::TestSuite::Build(env, "light-source native tests", {
        &ExecutorSpec,
        &StyleUtilsSpec,
    });
}

} // namespace ls
