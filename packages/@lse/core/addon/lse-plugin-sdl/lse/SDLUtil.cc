/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/SDLUtil.h>
#include <lse/Renderer.h>
#include <lse/Math.h>

namespace lse {

class SDLTextureBridge final : public Texture::Bridge {
 public:
  ~SDLTextureBridge() override = default;

  int32_t GetWidth(void* platformTextureRef) const noexcept override;
  int32_t GetHeight(void* platformTextureRef) const noexcept override;
  bool Lock(void* platformTextureRef, void** buffer, int32_t* pitch) noexcept override;
  void Unlock(void* platformTextureRef) noexcept override;
  bool Update(void* platformTextureRef, const uint8_t* buffer, int32_t length) override;
  PixelFormat GetPixelFormat(void* platformTextureRef) const noexcept override;
  Texture::Type GetType(void* platformTextureRef) const noexcept override;
  void Destroy(void* platformTextureRef) noexcept override;
};

static const SDL_Point kCenterPoint{ 0, 0 };
static SDLTextureBridge sTextureBridge{};

Texture CreateTexture(
    SDL_Renderer* renderer, int32_t width, int32_t height, Texture::Type type,
    PixelFormat format) noexcept {
  auto texturePtr{ SDL2::SDL_CreateTexture(
      renderer, ToSDLPixelFormat(format), ToSDLTextureAccess(type), width, height) };

  if (texturePtr) {
    SDL2::SDL_SetTextureBlendMode(texturePtr, SDL_BLENDMODE_BLEND);
  }

  return {
      texturePtr,
      &sTextureBridge
  };
}

SDL_Texture* DestroyTexture(SDL_Texture* texture) noexcept {
  if (texture) {
    SDL2::SDL_DestroyTexture(texture);
  }

  return nullptr;
}

SDL_Renderer* DestroyRenderer(SDL_Renderer* renderer) noexcept {
  if (renderer) {
    SDL2::SDL_DestroyRenderer(renderer);
  }

  return nullptr;
}

int32_t SDLTextureBridge::GetWidth(void* platformTextureRef) const noexcept {
  int32_t width{ 0 };

  if (platformTextureRef) {
    SDL2::SDL_QueryTexture(static_cast<SDL_Texture*>(platformTextureRef), nullptr, nullptr, &width, nullptr);
  }

  return width;
}

int32_t SDLTextureBridge::GetHeight(void* platformTextureRef) const noexcept {
  int32_t height{ 0 };

  if (platformTextureRef) {
    SDL2::SDL_QueryTexture(static_cast<SDL_Texture*>(platformTextureRef), nullptr, nullptr, nullptr, &height);
  }

  return height;
}

bool SDLTextureBridge::Lock(void* platformTextureRef, void** buffer, int32_t* pitch) noexcept {
  auto p{ static_cast<SDL_Texture*>(platformTextureRef) };

  return p ? SDL2::SDL_LockTexture(p, nullptr, buffer, pitch) == 0 : false;
}

void SDLTextureBridge::Unlock(void* platformTextureRef) noexcept {
  if (platformTextureRef) {
    SDL2::SDL_UnlockTexture(static_cast<SDL_Texture*>(platformTextureRef));
  }
}

bool SDLTextureBridge::Update(void* platformTextureRef, const uint8_t* buffer, int32_t length) {
  auto p{ static_cast<SDL_Texture*>(platformTextureRef) };

  return p ? SDL2::SDL_UpdateTexture(static_cast<SDL_Texture*>(p), nullptr, buffer, length) == 0 : false;
}

PixelFormat SDLTextureBridge::GetPixelFormat(void* platformTextureRef) const noexcept {
  uint32_t format{ SDL_PIXELFORMAT_UNKNOWN };

  if (platformTextureRef) {
    SDL2::SDL_QueryTexture(static_cast<SDL_Texture*>(platformTextureRef), &format, nullptr, nullptr, nullptr);
  }

  return ToPixelFormat(format);
}

Texture::Type SDLTextureBridge::GetType(void* platformTextureRef) const noexcept {
  int32_t access{ SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC };

  if (platformTextureRef) {
    SDL2::SDL_QueryTexture(static_cast<SDL_Texture*>(platformTextureRef), nullptr, &access, nullptr, nullptr);
  }

  return ToTextureType(access);
}

void SDLTextureBridge::Destroy(void* platformTextureRef) noexcept {
  DestroyTexture(static_cast<SDL_Texture*>(platformTextureRef));
}

void SDLSetDrawColor(SDL_Renderer* renderer, const RenderFilter& filter) noexcept {
  SDL2::SDL_SetRenderDrawColor(renderer, filter.tint.r, filter.tint.g, filter.tint.b, filter.tint.a);
}

void SDLSetTextureTint(SDL_Texture* texture, const RenderFilter& filter) noexcept {
  SDL2::SDL_SetTextureColorMod(texture, filter.tint.r, filter.tint.g, filter.tint.b);
  SDL2::SDL_SetTextureAlphaMod(texture, filter.tint.a);
}

SDL_RendererFlip SDLGetRenderFlip(const RenderFilter& filter) noexcept {
  int32_t flip{SDL_FLIP_NONE};

  if (filter.flipH) {
    flip |= SDL_FLIP_HORIZONTAL;
  }

  if (filter.flipV) {
    flip |= SDL_FLIP_VERTICAL;
  }

  return static_cast<SDL_RendererFlip>(flip);
}

} // namespace lse
