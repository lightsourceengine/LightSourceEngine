/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>
#include <ls/Style.h>

namespace ls {
namespace bindings {

Napi::Value BoxColor(const Napi::Env& env, const std17::optional<color_t>& color);

std17::optional<color_t> UnboxColor(const Napi::Env& env, const Napi::Value& value);
std17::optional<StyleValue> UnboxStyleValue(const Napi::Env& env, const Napi::Value& value);

} // namespace bindings
} // namespace ls

