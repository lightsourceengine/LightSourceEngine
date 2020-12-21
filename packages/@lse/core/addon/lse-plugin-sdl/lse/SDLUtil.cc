/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/SDLUtil.h>

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

void SetTextureTintColor(SDL_Texture* texture, color_t color) noexcept {
  SDL2::SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
  SDL2::SDL_SetTextureAlphaMod(texture, color.a);
}

void DrawImage(
    SDL_Renderer* renderer, SDL_Texture* texture, const Rect& rect, const Matrix& transform,
    color_t tintColor) noexcept {
  if (!texture) {
    return;
  }

  // TODO: snap to pixel grid?
  const SDL_Rect destRect{
      static_cast<int32_t>(rect.x + transform.GetTranslateX()),
      static_cast<int32_t>(rect.y + transform.GetTranslateY()),
      static_cast<int32_t>(rect.width * transform.GetScaleX()),
      static_cast<int32_t>(rect.height * transform.GetScaleY())
  };

  SetTextureTintColor(texture, tintColor);

  SDL2::SDL_RenderCopyEx(
      renderer, texture, nullptr, &destRect, transform.GetAxisAngleDeg(), &kCenterPoint, SDL_FLIP_NONE);
}

void DrawImage(
    SDL_Renderer* renderer, SDL_Texture* texture, const Rect& srcRect, const Rect& destRect,
    const Matrix& transform, color_t tintColor) noexcept {
  if (!texture) {
    return;
  }

  // TODO: snap to pixel grid?
  const SDL_Rect sdlDestRect{
      static_cast<int32_t>(destRect.x + transform.GetTranslateX()),
      static_cast<int32_t>(destRect.y + transform.GetTranslateY()),
      static_cast<int32_t>(destRect.width * transform.GetScaleX()),
      static_cast<int32_t>(destRect.height * transform.GetScaleY())
  };

  // TODO: snap to pixel grid?
  const SDL_Rect sdlSrcRect{
      static_cast<int32_t>(srcRect.x),
      static_cast<int32_t>(srcRect.y),
      static_cast<int32_t>(srcRect.width),
      static_cast<int32_t>(srcRect.height)
  };

  SetTextureTintColor(texture, tintColor);

  SDL2::SDL_RenderCopyEx(
      renderer, texture, &sdlSrcRect, &sdlDestRect, transform.GetAxisAngleDeg(), &kCenterPoint, SDL_FLIP_NONE);
}

void DrawImage(
    SDL_Renderer* renderer, SDL_Texture* texture, const EdgeRect& capInsets, const Rect& rect,
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
  SDL2::SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);

  // Top row

  srcRect = { 0, 0, capInsets.left, capInsets.top };
  destRect = { x, y, capInsets.left, capInsets.top };

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

  srcRect = { capInsets.left, 0, textureWidth - capInsets.left - capInsets.right, capInsets.top };
  destRect = { x + capInsets.left, y, w - capInsets.left - capInsets.right, capInsets.top };

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

  srcRect = { textureWidth - capInsets.right, 0, capInsets.right, capInsets.top };
  destRect = { x + w - capInsets.right, y, capInsets.right, capInsets.top };

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

  // Middle row

  srcRect = { 0, capInsets.top, capInsets.left, textureHeight - capInsets.top - capInsets.bottom };
  destRect = { x, y + capInsets.top, capInsets.left, h - capInsets.top - capInsets.bottom };

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

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

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

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

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

  // Bottom row

  srcRect = { 0, textureHeight - capInsets.bottom, capInsets.left, capInsets.bottom };
  destRect = { x, y + h - capInsets.bottom, capInsets.left, capInsets.bottom };

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

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

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

  srcRect = { textureWidth - capInsets.right, textureHeight - capInsets.bottom, capInsets.right, capInsets.bottom };
  destRect = { x + w - capInsets.right, y + h - capInsets.bottom, capInsets.right, capInsets.bottom };

  SDL2::SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
}

void DrawBorder(
    SDL_Renderer* renderer, SDL_Texture* fillRectTexture, const Rect& rect, const EdgeRect& border,
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

    SDL2::SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
  }

  // Bottom edge
  if (border.bottom > 0) {
    int32_t yShift{ h - border.bottom };
    SDL_Rect edge{ x, y + yShift, w, border.bottom };
    SDL_Point center{ 0, -yShift };

    SDL2::SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
  }

  // Left edge
  if (border.left > 0) {
    int32_t yShift{ border.top };
    SDL_Rect edge{ x, y + yShift, border.left, h - border.top - border.bottom };
    SDL_Point center{ 0, -yShift };

    SDL2::SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
  }

  // Right edge
  if (border.right > 0) {
    int32_t xShift{ w - border.right };
    int32_t yShift{ border.top };
    SDL_Rect edge{ x + xShift, y + yShift, border.right, h - border.top - border.bottom };
    SDL_Point center{ -xShift, -yShift };

    SDL2::SDL_RenderCopyEx(renderer, fillRectTexture, nullptr, &edge, rotate, &center, SDL_FLIP_NONE);
  }
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

} // namespace lse
