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

    int32_t GetWidth() const override;

    int32_t GetHeight() const override;

    void Reset() override;

    void Present() override;

    void Shift(float x, float y) override;

    void Unshift() override;

    void PushClipRect(const Rect& rect) override;

    void PopClipRect() override;

    void DrawFillRect(const Rect& rect, const int64_t fillColor) override;

    void DrawBorder(const Rect& rect, const EdgeRect& border, const int64_t borderColor) override;

    void DrawImage(const uint32_t textureId, const Rect& rect, const int64_t tintColor) override;

    void DrawImage(
        const uint32_t textureId, const Rect& rect, const EdgeRect& capInsets, const uint32_t tintColor) override;

    void DrawQuad(
        const uint32_t textureId, const Rect& srcRect, const Rect& destRect, const int64_t tintColor) override;

    void ClearScreen(const int64_t color) override;

    uint32_t CreateTexture(const int32_t width, const int32_t height) override;

    uint32_t CreateTexture(const int32_t width, const int32_t height,
        const uint8_t* source, PixelFormat sourceFormat) override;

    LockTextureInfo LockTexture(const uint32_t textureId) override;

    void DestroyTexture(const uint32_t textureId) override;

    PixelFormat GetTextureFormat() const override { return PixelFormatRGBA; }

 private:
    static uint32_t nextTextureId;
};

} // namespace ls
