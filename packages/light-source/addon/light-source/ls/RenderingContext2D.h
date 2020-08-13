/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <ls/Color.h>
#include <ls-ctx.h>

namespace ls {

/**
 * Vector drawing operations.
 *
 * The interface of this class follows the standard CanvasRenderingContext2D. The class is used to render stylized
 * backgrounds of elements and text.
 */
class RenderingContext2D {
 public:
    RenderingContext2D() noexcept;
    virtual ~RenderingContext2D() noexcept;

    void Reset() noexcept;
    void Reset(uint8_t* buffer, int32_t width, int32_t height, int32_t pitch = 0) noexcept;

    void SetColor(color_t color) noexcept;
    void SetLineWidth(float width) noexcept;

    void BeginPath() noexcept;
    void ClosePath() noexcept;
    void MoveTo(float x, float y) noexcept;
    void LineTo(float x, float y) noexcept;
    void QuadTo(float cx, float cy, float x, float y) noexcept;
    void ArcTo(float x1, float y1, float x2, float y2, float radius) noexcept;

    void Fill() noexcept;
    void Stroke() noexcept;

    void Save() noexcept;
    void Restore() noexcept;

 private:
    Ctx* ctx{};
};

} // namespace ls
