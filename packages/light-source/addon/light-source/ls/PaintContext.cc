/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "PaintContext.h"
#include <ls/Surface.h>

namespace ls {

PaintContext::PaintContext() noexcept {
    this->ctx = ctx_new_for_framebuffer(nullptr, 0, 0, 0, CTX_FORMAT_RGBA8);
}

PaintContext::~PaintContext() noexcept {
    ctx_free(this->ctx);
}

Ctx* PaintContext::Context2D(const Surface& surface) noexcept {
    ctx_set_framebuffer(this->ctx, surface.Pixels(), surface.Width(), surface.Height(), surface.Pitch(),
        CTX_FORMAT_RGBA8);

    return this->ctx;
}

void PaintContext::Reset(Renderer* renderer) noexcept {
    this->renderer = renderer;
}

} // namespace ls
