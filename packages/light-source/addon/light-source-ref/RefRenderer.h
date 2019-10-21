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
    RefRenderer();
    virtual ~RefRenderer();

    bool SetRenderTarget(std::shared_ptr<Texture> renderTarget) override;
    void Present() override;

    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    PixelFormat GetTextureFormat() const override { return PixelFormatRGBA; }

    void Shift(float x, float y) override;
    void Unshift() override;
    void PushClipRect(const Rect& rect) override;
    void PopClipRect() override;

    void DrawFillRect(const Rect& rect, const int64_t fillColor) override;
    void DrawBorder(const Rect& rect, const EdgeRect& border, const int64_t borderColor) override;
    void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const int64_t tintColor) override;
    void DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) override;
    void DrawQuad(const std::shared_ptr<Texture>& texture, const Rect& srcRect, const Rect& destRect,
        const int64_t tintColor) override;
    void ClearScreen(const int64_t color) override;

    std::shared_ptr<Texture> CreateRenderTarget(const int32_t width, const int32_t height) override;
    std::shared_ptr<Texture> CreateTextureFromSurface(const Surface& surface) override;
    std::shared_ptr<Texture> CreateTexture(const int32_t width, const int32_t height) override;
};

} // namespace ls
