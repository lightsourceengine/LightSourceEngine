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

    virtual void ProcessEvents() = 0;
    virtual std::unique_ptr<SceneAdapter> CreateSceneAdapter(const SceneAdapterConfig& config) = 0;
};

} // namespace ls
