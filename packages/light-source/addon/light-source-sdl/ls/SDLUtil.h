/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ls/PixelFormat.h>
#include <ls/ByteOrder.h>
#include <ls/Matrix.h>
#include <ls/Rect.h>
#include <ls/Color.h>
#include <ls/Texture.h>
#include <SDL2.include>

namespace ls {

// Texture operations

Texture CreateTexture(SDL_Renderer* renderer, int32_t width, int32_t height, Texture::Type type,
        PixelFormat format) noexcept;
SDL_Texture* DestroyTexture(SDL_Texture* texture) noexcept;
SDL_Renderer* DestroyRenderer(SDL_Renderer* renderer) noexcept;
/** Set the color mod and alpha mod of the texture used for render copy operations.  */
void SetTextureTintColor(SDL_Texture* texture, color_t color) noexcept;

// Render operations

void DrawImage(SDL_Renderer* renderer, SDL_Texture* texture, const Rect& rect, const Matrix& transform,
        color_t tintColor) noexcept;
void DrawImage(SDL_Renderer* renderer, SDL_Texture* texture, const Rect& srcRect, const Rect& destRect,
        const Matrix& transform, color_t tintColor) noexcept;
void DrawImage(SDL_Renderer* renderer, SDL_Texture* texture, const EdgeRect& capInsets, const Rect& rect,
        const Matrix& transform, color_t tintColor) noexcept;
void DrawBorder(SDL_Renderer* renderer, SDL_Texture* fillRectTexture, const Rect& rect, const EdgeRect& border,
        const Matrix& transform, color_t color) noexcept;

// Type conversions

/**
 * Convert ls::Rect to SDL_Rect.
 *
 * The ls::Rect is hard snapped to the pixel grid using floor().
 */
constexpr SDL_Rect ToSDLRect(const Rect& rect) noexcept {
    return {
        static_cast<int32_t>(rect.x),
        static_cast<int32_t>(rect.y),
        static_cast<int32_t>(rect.width),
        static_cast<int32_t>(rect.height),
    };
}

/** @return PixelFormat representation or PixelFormatUnknown if a PixelFormat cannot be found. */
constexpr PixelFormat ToPixelFormat(uint32_t pixelFormat) noexcept {
#if LS_BYTE_ORDER == LS_BIG_ENDIAN
    switch (pixelFormat) {
        case SDL_PIXELFORMAT_RGBA8888:
            return PixelFormatRGBA;
        case SDL_PIXELFORMAT_ARGB8888:
            return PixelFormatARGB;
        case SDL_PIXELFORMAT_BGRA8888:
            return PixelFormatBGRA;
        case SDL_PIXELFORMAT_ABGR8888:
            return PixelFormatABGR;
        default:
            return PixelFormatUnknown;
    }
#else
    switch (pixelFormat) {
        case SDL_PIXELFORMAT_ABGR8888:
            return PixelFormatRGBA;
        case SDL_PIXELFORMAT_BGRA8888:
            return PixelFormatARGB;
        case SDL_PIXELFORMAT_ARGB8888:
            return PixelFormatBGRA;
        case SDL_PIXELFORMAT_RGBA8888:
            return PixelFormatABGR;
        default:
            return PixelFormatUnknown;
    }
#endif
}

/** @returns SDL pixel format representation; otherwise SDL_PIXELFORMAT_UNKNOWN */
constexpr uint32_t ToSDLPixelFormat(PixelFormat format) noexcept {
#if LS_BYTE_ORDER == LS_BIG_ENDIAN
    switch (format) {
        case PixelFormatRGBA:
            return SDL_PIXELFORMAT_RGBA8888;
        case PixelFormatARGB:
            return SDL_PIXELFORMAT_ARGB8888;
        case PixelFormatBGRA:
            return SDL_PIXELFORMAT_BGRA8888;
        case PixelFormatABGR:
            return SDL_PIXELFORMAT_ABGR8888;
        default:
            return PixelFormatUnknown;
    }
#else
    switch (format) {
        case PixelFormatRGBA:
            return SDL_PIXELFORMAT_ABGR8888;
        case PixelFormatARGB:
            return SDL_PIXELFORMAT_BGRA8888;
        case PixelFormatBGRA:
            return SDL_PIXELFORMAT_ARGB8888;
        case PixelFormatABGR:
            return SDL_PIXELFORMAT_RGBA8888;
        default:
            return PixelFormatUnknown;
    }
#endif
}

/** @returns SDL texture access; otherwise SDL_TEXTUREACCESS_STATIC if unknown  */
constexpr SDL_TextureAccess ToSDLTextureAccess(Texture::Type type) noexcept {
    switch (type) {
        case Texture::Type::Lockable:
            return SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING;
        case Texture::Type::RenderTarget:
            return SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET;
        case Texture::Type::Updatable:
        default:
            return SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC;
    }
}

/** @returns ls::Texture::Type; otherwise Texture::Type::Updatable if unknown */
constexpr Texture::Type ToTextureType(int32_t textureAccess) noexcept {
    switch (textureAccess) {
        case SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING:
            return Texture::Type::Lockable;
        case SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET:
            return Texture::Type::RenderTarget;
        case SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC:
        default:
            return Texture::Type::Updatable;
    }
}

} // namespace ls
