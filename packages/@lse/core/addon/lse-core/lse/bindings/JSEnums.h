/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi-ext.h>

namespace lse {
namespace bindings {

Napi::Object NewStyleUnitEnum(Napi::Env env);
Napi::Object NewStyleTransformEnum(Napi::Env env);
Napi::Object NewStyleAnchorEnum(Napi::Env env);
Napi::Object NewPluginIdEnum(Napi::Env env);

} // namespace bindings
} // namespace lse
