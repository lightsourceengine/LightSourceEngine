/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Renderer.h>
#include <lse/SDL2.h>
#include <vector>

namespace lse {

class SDLRenderer final : public Renderer {
 public:
  SDLRenderer();
  ~SDLRenderer() override;

  int32_t GetWidth() const override { return this->width; }
  int32_t GetHeight() const override { return this->height; }
  PixelFormat GetTextureFormat() const override { return this->textureFormat; }

  bool SetRenderTarget(const Texture& newRenderTarget) override;
  void Reset() override;
  void FillRenderTarget(color_t color) override;
  void Present() override;
  void EnabledClipping(const Rect& rect) override;
  void DisableClipping() override;

  void DrawFillRect(const Rect& rect, const Matrix& transform, color_t fillColor) override;
  void DrawBorder(const Rect& rect, const EdgeRect& border, const Matrix& transform, color_t fillColor) override;
  void DrawImage(
      const Texture& texture, const Rect& rect, const Matrix& transform,
      color_t tintColor) override;
  void DrawImage(
      const Texture& texture, const Rect& destRect, const Rect& srcRect,
      const Matrix& transform, color_t tintColor) override;
  void DrawImage(
      const Texture& texture, const EdgeRect& capInsets, const Rect& rect,
      const Matrix& transform, color_t tintColor) override;

  Texture CreateTexture(int32_t width, int32_t height, Texture::Type type) override;

  void Attach(SDL_Window* window);
  void Detach();
  void Destroy();

 private:
  void ResetInternal(const Texture& newRenderTarget);
  void SetRenderDrawColor(color_t color) noexcept;
  void UpdateTextureFormats(const SDL_RendererInfo& info) noexcept;

 private:
  SDL_Renderer* renderer{};
  PixelFormat textureFormat{ PixelFormatUnknown };
  color_t drawColor{};
  Texture fillRectTexture{};
  Texture renderTarget{};
  int32_t width{ 0 };
  int32_t height{ 0 };
};

} // namespace lse
