/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace lse {
namespace bindings {

napi_value LoadSDLPlatformPlugin(napi_env env) noexcept;

} // namespace bindings
} // namespace lse
