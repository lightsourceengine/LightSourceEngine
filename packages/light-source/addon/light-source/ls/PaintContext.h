/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls-ctx.h>

namespace ls {

class Renderer;
class Surface;

class PaintContext {
 public:
    PaintContext() noexcept;
    ~PaintContext() noexcept;

    Renderer* renderer{};
    Ctx* Context2D(const Surface& surface) noexcept;
    void Reset(Renderer* renderer) noexcept;

 private:
    Ctx* ctx{};
};

} // namespace ls
