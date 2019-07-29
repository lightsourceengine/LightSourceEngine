/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <memory>
#include "SceneAdapter.h"

namespace ls {

class StageAdapter {
 public:
    virtual ~StageAdapter() = default;

    virtual Napi::Value ProcessEvents(const Napi::CallbackInfo& info) = 0;
    virtual void ProcessEvents() = 0;
    virtual std::shared_ptr<SceneAdapter> CreateSceneAdapter(int32_t displayId) = 0;
};

} // namespace ls
