/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefRenderer.h"

namespace ls {

class RefTexture : public Texture {
 public:
    RefTexture(const int32_t width, const int32_t height, const bool isRenderTarget, const bool canUpdate,
        const bool canLock)
        : width(width), height(height), isRenderTarget(isRenderTarget), canUpdate(canUpdate), canLock(canLock) {
    }
    virtual ~RefTexture() noexcept = default;

    bool IsAttached() const noexcept override { return true; }
    int32_t GetWidth() const noexcept override { return this->width; }
    int32_t GetHeight() const noexcept override { return this->height; }
    PixelFormat GetFormat() const noexcept override { return PixelFormatRGBA; }
    bool IsRenderTarget() const noexcept override { return this->isRenderTarget; }
    bool CanUpdate() const noexcept override { return this->canUpdate; }
    bool Update(const Surface& surface) noexcept override { return true; }
    bool CanLock() const noexcept override { return this->canLock; }
    Surface Lock() noexcept override { return {}; }

 private:
    int32_t width;
    int32_t height;
    bool isRenderTarget;
    bool canUpdate;
    bool canLock;
};

RefRenderer::RefRenderer() {
}

RefRenderer::~RefRenderer() {
}

int32_t RefRenderer::GetWidth() const {
    return 0;
}

int32_t RefRenderer::GetHeight() const {
    return 0;
}

void RefRenderer::Present() {
}

void RefRenderer::Shift(float x, float y) {
}

void RefRenderer::Unshift() {
}

void RefRenderer::PushClipRect(const Rect& rect) {
}

void RefRenderer::PopClipRect() {
}

void RefRenderer::DrawFillRect(const Rect& rect, const int64_t fillColor) {
}

void RefRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const int64_t borderColor) {
}

void RefRenderer::DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const int64_t tintColor) {
}

void RefRenderer::DrawImage(const std::shared_ptr<Texture>& texture, const Rect& rect, const EdgeRect& capInsets,
        const uint32_t tintColor) {
}

void RefRenderer::DrawQuad(const std::shared_ptr<Texture>& texture, const Rect& srcRect, const Rect& destRect,
        const int64_t tintColor) {
}

void RefRenderer::ClearScreen(const int64_t color) {
}

bool RefRenderer::SetRenderTarget(std::shared_ptr<Texture> renderTarget) {
    return true;
}

std::shared_ptr<Texture> RefRenderer::CreateRenderTarget(const int32_t width, const int32_t height) {
    return std::static_pointer_cast<Texture>(std::make_shared<RefTexture>(width, height, true, false, false));
}

std::shared_ptr<Texture> RefRenderer::CreateTextureFromSurface(const Surface& surface) {
    return std::static_pointer_cast<Texture>(std::make_shared<RefTexture>(
        surface.Width(), surface.Height(), false, false, false));
}

std::shared_ptr<Texture> RefRenderer::CreateTexture(const int32_t width, const int32_t height) {
    return std::static_pointer_cast<Texture>(std::make_shared<RefTexture>(width, height, false, true, true));
}

} // namespace ls
