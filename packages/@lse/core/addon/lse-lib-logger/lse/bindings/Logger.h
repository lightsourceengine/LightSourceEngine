/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi-ext.h>
#include <lse/Log.h>

namespace lse {
namespace bindings {

// logger js class creators

Napi::Object NewLoggerObject(Napi::Env env);
Napi::Object NewLogLevelEnum(Napi::Env env);

} // namespace bindings
} // namespace lse
