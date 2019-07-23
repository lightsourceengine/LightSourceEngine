/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {

class StageAdapter {
 public:
    virtual ~StageAdapter() = default;

    virtual Napi::Value ProcessEvents(const Napi::CallbackInfo& info) = 0;
    virtual Napi::Value CreateSceneAdapter(const Napi::CallbackInfo& info) = 0;
};

} // namespace ls
