/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/RefRenderer.h>

namespace ls {

static Texture::Bridge CreateTextureBridge() noexcept;
static Texture::Bridge sTextureBridge = CreateTextureBridge();

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

void RefRenderer::DrawFillRect(const Rect& rect, const Matrix& transform, uint32_t fillColor) {
}

void RefRenderer::DrawBorder(const Rect& rect, const EdgeRect& border, const Matrix& transform, uint32_t fillColor) {
}

void RefRenderer::DrawImage(const Texture& texture, const Rect& rect,
        const Matrix& transform, uint32_t tintColor) {
}

void RefRenderer::DrawImage(const Texture& texture, const EdgeRect& capInsets, const Rect& rect,
        const Matrix& transform, uint32_t tintColor) {
}

void RefRenderer::FillRenderTarget(const uint32_t color) {
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

static Texture::Bridge CreateTextureBridge() noexcept {
    return {
        // GetWidth()
        [](void* p) -> int32_t {
            return p ? static_cast<RefTexture*>(p)->width : 0;
        },
        // GetHeight()
        [](void* p) -> int32_t {
            return p ? static_cast<RefTexture*>(p)->height : 0;
        },
        // Lock()
        [](void* p, void** data, int32_t* pitch) -> bool {
            if (p) {
                auto texture{ static_cast<RefTexture*>(p) };
                auto actualPitch{ texture->width * 4 };

                texture->pixels = new uint8_t[texture->height * actualPitch];

                *data = texture->pixels;
                *pitch = actualPitch;

                return true;
            }

            return false;
        },
        // Unlock
        [](void* p) {
            if (p) {
                auto texture{ static_cast<RefTexture*>(p) };

                delete [] texture->pixels;
                texture->pixels = nullptr;
            }
        },
        // Update()
        [](void* p, const uint8_t* pixels, int32_t pitch) -> bool {
            return p != nullptr;
        },
        // GetPixelFormat()
        [](void* p) -> PixelFormat {
            return p ? static_cast<RefTexture*>(p)->format : PixelFormatUnknown;
        },
        // GetType()
        [](void* p) -> Texture::Type {
            return p ? static_cast<RefTexture*>(p)->type : Texture::Type::Updatable;
        },
        // Destroy()
        [](void* p) {
            delete static_cast<RefTexture*>(p);
        }
    };
}

} // namespace ls
