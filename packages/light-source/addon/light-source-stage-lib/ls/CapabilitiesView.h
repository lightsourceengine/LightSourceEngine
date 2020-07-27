/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>
#include <ls/StageAdapter.h>

namespace ls {

Napi::Object ToCapabilitiesView(Napi::Env env, const Capabilities& caps);

} // namespace ls
