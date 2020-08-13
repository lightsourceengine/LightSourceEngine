/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/RenderingContext2D.h>

namespace ls {

RenderingContext2D::RenderingContext2D() noexcept
: ctx(ctx_new_for_framebuffer(nullptr, 0, 0, 0, CTX_FORMAT_RGBA8)) {
}

RenderingContext2D::~RenderingContext2D() noexcept {
    ctx_free(this->ctx);
}

void RenderingContext2D::Reset() noexcept {
    this->Reset(nullptr, 0, 0, 0);
}

void RenderingContext2D::Reset(uint8_t* buffer, int32_t width, int32_t height, int32_t pitch) noexcept {
    if (pitch <= 0) {
        pitch = width * 4;
    }

    ctx_clear(this->ctx);
    ctx_set_framebuffer(this->ctx, buffer, width, height, pitch, CTX_FORMAT_RGBA8);
}

void RenderingContext2D::SetColor(color_t color) noexcept {
    ctx_set_rgba_u8(this->ctx, color.r, color.g, color.b, color.a);
}

void RenderingContext2D::SetLineWidth(float width) noexcept {
    ctx_set_line_width(this->ctx, width);
}

void RenderingContext2D::BeginPath() noexcept {
    ctx_new_path(this->ctx);
}

void RenderingContext2D::ClosePath() noexcept {
    ctx_close_path(this->ctx);
}

void RenderingContext2D::MoveTo(float x, float y) noexcept {
    ctx_move_to(this->ctx, x, y);
}

void RenderingContext2D::LineTo(float x, float y) noexcept {
    ctx_line_to(this->ctx, x, y);
}

void RenderingContext2D::QuadTo(float cx, float cy, float x, float y) noexcept {
    ctx_quad_to(this->ctx, cx, cy, x, y);
}

void RenderingContext2D::ArcTo(float x1, float y1, float x2, float y2, float radius) noexcept {
    ctx_arc_to(this->ctx, x1, y1, x2, y2, radius);
}

void RenderingContext2D::Fill() noexcept {
    ctx_fill(this->ctx);
}

void RenderingContext2D::Stroke() noexcept {
    ctx_stroke(this->ctx);
}

void RenderingContext2D::Save() noexcept {
    ctx_save(this->ctx);
}

void RenderingContext2D::Restore() noexcept {
    ctx_restore(this->ctx);
}

} // namespace ls
