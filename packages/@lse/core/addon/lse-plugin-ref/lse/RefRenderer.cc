/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/RefRenderer.h>

namespace lse {

class RefTextureBridge final : public Texture::Bridge {
 public:
    ~RefTextureBridge() override = default;

    int32_t GetWidth(void* platformTextureRef) const noexcept override;
    int32_t GetHeight(void* platformTextureRef) const noexcept override;
    bool Lock(void* platformTextureRef, void** buffer, int32_t* pitch) noexcept override;
    void Unlock(void* platformTextureRef) noexcept override;
    bool Update(void* platformTextureRef, const uint8_t* buffer, int32_t length) override;
    PixelFormat GetPixelFormat(void* platformTextureRef) const noexcept override;
    Texture::Type GetType(void* platformTextureRef) const noexcept override;
    void Destroy(void* platformTextureRef) noexcept override;
};

static RefTextureBridge sTextureBridge{};

struct RefTexture {
    int32_t width{};
    int32_t height{};
    uint8_t* pixels{};
    PixelFormat format{PixelFormat::PixelFormatUnknown};
    Texture::Type type{Texture::Type::Updatable};
};

int32_t RefRenderer::GetWidth() const {
    return 0;
}

int32_t RefRenderer::GetHeight() const {
    return 0;
}

void RefRenderer::Present() {
}

void RefRenderer::EnabledClipping(const Rect& rect) {
}

void RefRenderer::DisableClipping() {
}

void RefRenderer::DrawFillRect(const Rect& rect, const Matrix& transform, color_t fillColor) {
}

void RefRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const Matrix& transform, color_t fillColor) {
}

void RefRenderer::DrawImage(const Texture& texture, const Rect& rect, const Matrix& transform, color_t tintColor) {
}

void RefRenderer::DrawImage(const Texture& texture, const Rect& srcRect, const Rect& destRect, const Matrix& transform,
        color_t tintColor) {
}

void RefRenderer::DrawImage(const Texture& texture, const EdgeRect& capInsets, const Rect& rect,
        const Matrix& transform, color_t tintColor) {
}

void RefRenderer::FillRenderTarget(const color_t color) {
}

bool RefRenderer::SetRenderTarget(const Texture& renderTarget) {
    return true;
}

void RefRenderer::Reset() {
}

Texture RefRenderer::CreateTexture(int32_t width, int32_t height, Texture::Type type) {
    return {
        new RefTexture{ width, height, nullptr, this->GetTextureFormat(), type },
        &sTextureBridge
    };
}

int32_t RefTextureBridge::GetWidth(void* platformTextureRef) const noexcept {
    return platformTextureRef ? static_cast<RefTexture*>(platformTextureRef)->width : 0;
}

int32_t RefTextureBridge::GetHeight(void* platformTextureRef) const noexcept {
    return platformTextureRef ? static_cast<RefTexture*>(platformTextureRef)->height : 0;
}

bool RefTextureBridge::Lock(void* platformTextureRef, void** buffer, int32_t* pitch) noexcept {
    if (platformTextureRef) {
        auto texture{ static_cast<RefTexture*>(platformTextureRef) };
        auto actualPitch{ texture->width * 4 };

        texture->pixels = new uint8_t[texture->height * actualPitch];

        *buffer = texture->pixels;
        *pitch = actualPitch;

        return true;
    }

    *buffer = nullptr;
    *pitch = 0;

    return false;
}

void RefTextureBridge::Unlock(void* platformTextureRef) noexcept {
    if (platformTextureRef) {
        auto texture{ static_cast<RefTexture*>(platformTextureRef) };

        delete [] texture->pixels;
        texture->pixels = nullptr;
    }
}

bool RefTextureBridge::Update(void* platformTextureRef, const uint8_t* buffer, int32_t length) {
    return platformTextureRef != nullptr;
}

PixelFormat RefTextureBridge::GetPixelFormat(void* platformTextureRef) const noexcept {
    return platformTextureRef ? static_cast<RefTexture*>(platformTextureRef)->format : PixelFormatUnknown;
}

Texture::Type RefTextureBridge::GetType(void* platformTextureRef) const noexcept {
    return platformTextureRef ? static_cast<RefTexture*>(platformTextureRef)->type : Texture::Type::Updatable;
}

void RefTextureBridge::Destroy(void* platformTextureRef) noexcept {
    delete static_cast<RefTexture*>(platformTextureRef);
}

} // namespace lse
