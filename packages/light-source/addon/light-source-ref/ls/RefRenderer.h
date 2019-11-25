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
    virtual ~RefRenderer() = default;

    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    PixelFormat GetTextureFormat() const override { return PixelFormatRGBA; }

    bool SetRenderTarget(std::shared_ptr<Texture> renderTarget) override;
    void Present() override;
    void SetClipRect(const Rect& rect) override;
    void ClearClipRect() override;

    void DrawFillRect(const Rect& rect, const Matrix& transform, const uint32_t fillColor) override;
    void DrawBorder(const Rect& rect, const EdgeRect& border, const uint32_t borderColor) override;
    void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const uint32_t tintColor) override;
    void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect,
        const Matrix& transform, uint32_t tintColor) override;
    void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) override;
    void FillRenderTarget(const uint32_t color) override;

    std::shared_ptr<Texture> CreateRenderTarget(const int32_t width, const int32_t height) override;
    std::shared_ptr<Texture> CreateTextureFromSurface(const Surface& surface) override;
    std::shared_ptr<Texture> CreateTexture(const int32_t width, const int32_t height) override;
};

} // namespace ls
