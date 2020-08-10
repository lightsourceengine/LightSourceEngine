/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/Renderer.h>

namespace ls {

class RefRenderer : public Renderer {
 public:
    ~RefRenderer() override = default;

    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    PixelFormat GetTextureFormat() const override { return PixelFormatRGBA; }

    bool SetRenderTarget(const Texture& newRenderTarget) override;
    void Reset() override;
    void Present() override;
    void EnabledClipping(const Rect& rect) override;
    void DisableClipping() override;

    void DrawFillRect(const Rect& rect, const Matrix& transform, color_t fillColor) override;
    void DrawBorder(const Rect& rect, const EdgeRect& border, const Matrix& transform, color_t fillColor) override;
    void DrawImage(const Texture& texture, const Rect& rect, const Matrix& transform,
            color_t tintColor) override;
    void DrawImage(const Texture& texture, const EdgeRect& capInsets, const Rect& rect,
            const Matrix& transform, color_t tintColor) override;

    void FillRenderTarget(color_t color) override;

    Texture CreateTexture(int32_t width, int32_t height, Texture::Type type) override;
};

} // namespace ls
