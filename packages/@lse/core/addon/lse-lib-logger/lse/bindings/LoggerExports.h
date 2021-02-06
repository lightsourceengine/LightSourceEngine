/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <node_api.h>

namespace lse {
namespace bindings {

napi_value NewLoggerObject(napi_env env) noexcept;
napi_value NewLogLevelEnum(napi_env env) noexcept;

} // namespace bindings
} // namespace lse
