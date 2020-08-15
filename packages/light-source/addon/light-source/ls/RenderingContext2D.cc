/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/RenderingContext2D.h>

namespace ls {

RenderingContext2D::RenderingContext2D() noexcept {
}

RenderingContext2D::~RenderingContext2D() noexcept {
}

void RenderingContext2D::End() noexcept {
    this->context.end();
}

void RenderingContext2D::Begin(uint8_t* buffer, int32_t width, int32_t height, int32_t pitch) noexcept {
    if (pitch <= 0) {
        pitch = width * 4;
    }

    this->target.createFromData(
        width, height, BLFormat::BL_FORMAT_PRGB32, buffer, pitch, nullptr, nullptr);
    this->context.begin(this->target);
}

void RenderingContext2D::SetColor(color_t color) noexcept {
    // TODO: why is r and b swapped?
    this->context.setFillStyle(BLRgba32(color.b, color.g, color.r, color.a));
}

void RenderingContext2D::SetLineWidth(float width) noexcept {
    this->context.setStrokeWidth(width);
}

void RenderingContext2D::SetFont(BLFont value) noexcept {
    this->font = std::move(value);
}

void RenderingContext2D::BeginPath() noexcept {
    this->path.reset();
}

void RenderingContext2D::ClosePath() noexcept {
    this->path.close();
}

void RenderingContext2D::MoveTo(float x, float y) noexcept {
    this->path.moveTo(x, y);
}

void RenderingContext2D::LineTo(float x, float y) noexcept {
    this->path.lineTo(x, y);
}

void RenderingContext2D::QuadTo(float cx, float cy, float x, float y) noexcept {
    this->path.quadTo(cx, cy, x, y);
}

void RenderingContext2D::FillPath() noexcept {
    this->context.fillPath(this->path);
}

void RenderingContext2D::FillText(float x, float y, const BLGlyphRun& text) noexcept {
    this->context.fillGlyphRun(BLPoint(x, y), this->font, text);
}

void RenderingContext2D::FillAll() noexcept {
    this->context.fillAll();
}

void RenderingContext2D::StrokePath() noexcept {
    this->context.strokePath(this->path);
}

} // namespace ls
