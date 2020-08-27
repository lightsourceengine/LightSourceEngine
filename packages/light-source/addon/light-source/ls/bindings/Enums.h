/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace ls {
namespace bindings {

Napi::Function NewStyleUnitClass(Napi::Env env);
Napi::Function NewStyleTransformClass(Napi::Env env);

} // namespace bindings
} // namespace ls
