/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>

namespace ls {

class Renderer;

class SceneAdapter {
 public:
    virtual ~SceneAdapter() = default;

    virtual void Attach() = 0;
    virtual void Detach() = 0;
    virtual void Resize(int32_t width, int32_t height, bool fullscreen) = 0;

    virtual Renderer* GetRenderer() const = 0;
};

} // namespace ls
