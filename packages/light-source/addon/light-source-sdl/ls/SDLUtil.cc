/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/SDLUtil.h>

#include <ls/PixelConversion.h>

namespace ls {

static Texture::Bridge CreateTextureBridge() noexcept;

static const SDL_Point kCenterPoint{ 0, 0 };
static Texture::Bridge sTextureBridge = CreateTextureBridge();

Texture CreateTexture(SDL_Renderer* renderer, int32_t width, int32_t height, Texture::Type type,
        PixelFormat format) noexcept {
    return {
        SDL_CreateTexture(renderer, ToSDLPixelFormat(format), ToSDLTextureAccess(type), width, height),
        &sTextureBridge
    };
}

SDL_Texture* DestroyTexture(SDL_Texture* texture) noexcept {
    if (texture) {
        SDL_DestroyTexture(texture);
    }

    return nullptr;
}

SDL_Renderer* DestroyRenderer(SDL_Renderer* renderer) noexcept {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    return nullptr;
}

void SetTextureTintColor(SDL_Texture* texture, color_t color) noexcept {
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);
}

void DrawImage(SDL_Renderer* renderer, SDL_Texture* texture, const Rect& rect, const Matrix& transform,
        color_t tintColor) noexcept {
    if (!texture) {
        return;
    }

    const SDL_Rect destRect{
        static_cast<int32_t>(rect.x + transform.GetTranslateX()),
        static_cast<int32_t>(rect.y + transform.GetTranslateY()),
        static_cast<int32_t>(rect.width * transform.GetScaleX()),
        static_cast<int32_t>(rect.height * transform.GetScaleY())
    };

    SetTextureTintColor(texture, tintColor);

    SDL_RenderCopyEx(renderer, texture, nullptr, &destRect, transform.GetAxisAngleDeg(), &kCenterPoint, SDL_FLIP_NONE);
}

void DrawImage(SDL_Renderer* renderer, SDL_Texture* texture, const EdgeRect& capInsets, const Rect& rect,
        const Matrix& transform, color_t tintColor) noexcept {
    if (!texture) {
        return;
    }

    const auto x{ static_cast<int32_t>(rect.x) };
    const auto y{ static_cast<int32_t>(rect.y) };
    const auto w{ static_cast<int32_t>(rect.width) };
    const auto h{ static_cast<int32_t>(rect.height) };

    int32_t textureWidth;
    int32_t textureHeight;
    SDL_Rect srcRect;
    SDL_Rect destRect;

    SetTextureTintColor(texture, tintColor);
    SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);

    // Top row

    srcRect = { 0, 0, capInsets.left, capInsets.top };
    destRect = { x, y, capInsets.left, capInsets.top };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    srcRect = { capInsets.left, 0, textureWidth - capInsets.left - capInsets.right, capInsets.top };
    destRect = {x + capInsets.left, y, w - capInsets.left - capInsets.right, capInsets.top };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, 0, capInsets.right, capInsets.top };
    destRect = {x + w - capInsets.right, y, capInsets.right, capInsets.top };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    // Middle row

    srcRect = { 0, capInsets.top, capInsets.left, textureHeight - capInsets.top - capInsets.bottom };
    destRect = { x, y + capInsets.top, capInsets.left, h - capInsets.top - capInsets.bottom };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    srcRect = {
        capInsets.left,
        capInsets.top,
        textureWidth - capInsets.left - capInsets.right,
        textureHeight - capInsets.top - capInsets.bottom
    };
    destRect = {
        x + capInsets.left,
        y + capInsets.top,
        w - capInsets.left - capInsets.right,
        h - capInsets.top - capInsets.bottom
    };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    srcRect = {
        textureWidth - capInsets.right,
        capInsets.top,
        capInsets.right,
        textureHeight - capInsets.top - capInsets.bottom
    };
    destRect = {
        x + w - capInsets.right,
        y + capInsets.top,
        capInsets.right,
        h - capInsets.top - capInsets.bottom
    };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    // Bottom row

    srcRect = { 0, textureHeight - capInsets.bottom, capInsets.left, capInsets.bottom };
    destRect = { x, y + h - capInsets.bottom, capInsets.left, capInsets.bottom };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    srcRect = {
        capInsets.left,
        textureHeight - capInsets.bottom,
        textureWidth - capInsets.left - capInsets.right,
        capInsets.bottom
    };
    destRect = {
        x + capInsets.left,
        y + h - capInsets.bottom,
        w - capInsets.left - capInsets.right,
        capInsets.bottom
    };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    srcRect = { textureWidth - capInsets.right, textureHeight - capInsets.bottom, capInsets.right, capInsets.bottom };
    destRect = {x + w - capInsets.right, y + h - capInsets.bottom, capInsets.right, capInsets.bottom };

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}

void DrawBorder(SDL_Renderer* renderer, SDL_Texture* fillRectTexture, const Rect& rect, const EdgeRect& border,
        const Matrix& transform, color_t fillColor) noexcept {
    // TODO: snap to pixel grid
    const auto x{ static_cast<int32_t>(rect.x + transform.GetTranslateX()) };
    const auto y{ static_cast<int32_t>(rect.y + transform.GetTranslateY()) };
    const auto w{ static_cast<int32_t>(rect.width * transform.GetScaleX()) };
    const auto h{ static_cast<int32_t>(rect.height * transform.GetScaleY()) };

    const double rotate{ transform.GetAxisAngleDeg() };

    SetTextureTintColor(fillRectTexture, fillColor);

    // Top edge
    if (border.top > 0) {
        SDL_Rect edge{ x, y, w, border.top };
        SDL_Point center{ 0, 0 };

        SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
    }

    // Bottom edge
    if (border.bottom > 0) {
        int32_t yShift{ h - border.bottom };
        SDL_Rect edge{ x, y + yShift, w, border.bottom };
        SDL_Point center{ 0, -yShift };

        SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
    }

    // Left edge
    if (border.left > 0) {
        int32_t yShift{ border.top };
        SDL_Rect edge{ x, y + yShift, border.left, h - border.top - border.bottom };
        SDL_Point center{ 0, -yShift };

        SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
    }

    // Right edge
    if (border.right > 0) {
        int32_t xShift{ w - border.right };
        int32_t yShift{ border.top };
        SDL_Rect edge{ x + xShift, y + yShift, border.right, h - border.top - border.bottom };
        SDL_Point center{ -xShift, -yShift };

        SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
    }
}

static Texture::Bridge CreateTextureBridge() noexcept {
    return {
        // GetWidth()
        [](void* p) -> int32_t {
            int32_t width{0};

            if (p) {
                SDL_QueryTexture(static_cast<SDL_Texture*>(p), nullptr, nullptr, &width, nullptr);
            }

            return width;
        },
        // GetHeight()
        [](void* p) -> int32_t {
            int32_t height{0};

            if (p) {
                SDL_QueryTexture(static_cast<SDL_Texture*>(p), nullptr, nullptr, nullptr, &height);
            }

            return height;
        },
        // Lock()
        [](void* p, void** data, int32_t* pitch) -> bool {
            if (p) {
                return SDL_LockTexture(static_cast<SDL_Texture*>(p), nullptr, data, pitch) == 0;
            }

            return false;
        },
        // Unlock()
        [](void* p) {
            if (p) {
                SDL_UnlockTexture(static_cast<SDL_Texture*>(p));
            }
        },
        // Update()
        [](void* p, const uint8_t* pixels, int32_t pitch) -> bool {
            return p ? SDL_UpdateTexture(static_cast<SDL_Texture*>(p), nullptr, pixels, pitch) == 0 : false;
        },
        // GetPixelFormat()
        [](void* p) -> PixelFormat {
            uint32_t format{ SDL_PIXELFORMAT_UNKNOWN };

            if (p) {
                SDL_QueryTexture(static_cast<SDL_Texture*>(p), &format, nullptr, nullptr, nullptr);
            }

            return ToPixelFormat(format);
        },
        // GetType()
        [](void* p) -> Texture::Type {
            int32_t access{ SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC };

            if (p) {
                SDL_QueryTexture(static_cast<SDL_Texture*>(p), nullptr, &access, nullptr, nullptr);
            }

            return ToTextureType(access);
        },
        // Destroy()
        [](void* p) {
            DestroyTexture(static_cast<SDL_Texture*>(p));
        }
    };
}

} // namespace ls
