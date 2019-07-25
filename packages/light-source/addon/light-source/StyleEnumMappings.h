/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {
namespace StyleEnumMappings {

Napi::Object Init(Napi::Env env, Napi::Object exports);

} // namespace StyleEnumMappings
} // namespace ls
