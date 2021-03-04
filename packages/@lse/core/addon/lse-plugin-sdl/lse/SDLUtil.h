/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/PixelFormat.h>
#include <lse/System.h>
#include <lse/Matrix.h>
#include <lse/Rect.h>
#include <lse/Color.h>
#include <lse/Texture.h>
#include <lse/SDL2.h>
#include <lse/math-ext.h>

namespace lse {

static_assert(sizeof(SDL_Rect) == sizeof(IntRect) && sizeof(SDL_Rect::x) == sizeof(IntRect::x),
    "SDL_Rect/IntRect mismatch");

struct RenderTransform;
struct RenderFilter;

// Renderer/Drawing utilities

SDL_Renderer* DestroyRenderer(SDL_Renderer* renderer) noexcept;
void SDLSetDrawColor(SDL_Renderer* renderer, const RenderFilter& filter) noexcept;
void SDLSetTextureTint(SDL_Texture* texture, const RenderFilter& filter) noexcept;
SDL_RendererFlip SDLGetRenderFlip(const RenderFilter& filter) noexcept;

// Type conversions

/** @return PixelFormat representation or PixelFormatUnknown if a PixelFormat cannot be found. */
constexpr PixelFormat ToPixelFormat(uint32_t pixelFormat) noexcept {
  if (kIsBigEndian) {
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
  } else {
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
  }
}

/** @returns SDL pixel format representation; otherwise SDL_PIXELFORMAT_UNKNOWN */
constexpr uint32_t ToSDLPixelFormat(PixelFormat format) noexcept {
#if LSE_BYTE_ORDER == LSE_BIG_ENDIAN
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

/** @returns lse::Texture::Type; otherwise Texture::Type::Updatable if unknown */
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

template<typename T>
T SDLSnapToPixelGrid(const Rect& box) noexcept;

template<>
inline SDL_Rect SDLSnapToPixelGrid(const Rect& box) noexcept {
  return {
    SnapToPixelGrid<int32_t>(box.x),
    SnapToPixelGrid<int32_t>(box.y),
    SnapToPixelGrid<int32_t>(box.width),
    SnapToPixelGrid<int32_t>(box.height)
  };
}

template<>
inline SDL_FRect SDLSnapToPixelGrid(const Rect& box) noexcept {
  return {
    SnapToPixelGrid<float>(box.x),
    SnapToPixelGrid<float>(box.y),
    SnapToPixelGrid<float>(box.width),
    SnapToPixelGrid<float>(box.height)
  };
}

} // namespace lse
