/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-unit.h>

namespace ls {

void ThreadPoolSpec(Napi::TestSuite* parent);
void StyleSpec(Napi::TestSuite* parent);
void StyleContextSpec(Napi::TestSuite* parent);

inline
Napi::Value LightSourceTestSuite(Napi::Env env) {
    return Napi::TestSuite::Build(env, "light-source native tests", {
        &ThreadPoolSpec,
        &StyleSpec,
        &StyleContextSpec,
    });
}

} // namespace ls
