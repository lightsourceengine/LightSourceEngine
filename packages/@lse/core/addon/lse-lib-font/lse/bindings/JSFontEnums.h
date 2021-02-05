/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>

namespace lse {
namespace bindings {

/**
 * Javascript bindings for Font related enums.
 */

Napi::Value NewFontStatusEnum(const Napi::Env& env);
Napi::Value NewFontStyleEnum(const Napi::Env& env);
Napi::Value NewFontWeightEnum(const Napi::Env& env);

} // namespace bindings
} // namespace lse
