/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <ls/Color.h>
#include <blend2d.h>

namespace ls {

class Renderer;

/**
 * Vector drawing operations.
 *
 * The interface of this class follows the standard CanvasRenderingContext2D. The class is used to render stylized
 * backgrounds of elements and text.
 */
class RenderingContext2D {
 public:
    virtual ~RenderingContext2D() noexcept = default;

    void Begin(uint8_t* buffer, int32_t width, int32_t height, int32_t pitch = 0) noexcept;
    void End() noexcept;

    void SetColor(color_t color) noexcept;
    void SetLineWidth(float width) noexcept;
    void SetFont(BLFont font) noexcept;

    void BeginPath() noexcept;
    void ClosePath() noexcept;
    void MoveTo(float x, float y) noexcept;
    void LineTo(float x, float y) noexcept;
    void QuadTo(float cx, float cy, float x, float y) noexcept;

    void FillPath() noexcept;
    void StrokePath() noexcept;
    void FillText(float x, float y, const char* text) noexcept;
    void FillText(float x, float y, const BLGlyphRun& text) noexcept;
    void FillAll() noexcept;

 public:
    Renderer* renderer{};

 private:
    BLContext context{};
    BLImage target{};
    BLPath path{};
    BLFont font{};
};

} // namespace ls
