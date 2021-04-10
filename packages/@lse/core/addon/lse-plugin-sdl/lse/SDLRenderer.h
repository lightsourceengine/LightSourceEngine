/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#pragma once

#include <lse/Renderer.h>
#include <vector>
#include <lse/SDL2.h>
#include <phmap.h>

namespace lse {

class SDLRenderer final : public Renderer, public std::enable_shared_from_this<SDLRenderer> {
 public:
  SDLRenderer();
  ~SDLRenderer() override;

  static std::shared_ptr<SDLRenderer> New();

  int32_t GetWidth() const noexcept override { return this->width; }
  int32_t GetHeight() const noexcept override { return this->height; }
  PixelFormat GetTextureFormat() const noexcept override { return this->textureFormat; }

  bool SetRenderTarget(Texture* texture) noexcept override;
  void Reset() noexcept override;
  void Clear(color_t color) noexcept override;
  void Present() noexcept override;
  void EnabledClipping(const Rect& rect) noexcept override;
  void DisableClipping() noexcept override;

  Texture* CreateTexture(int32_t width, int32_t height, Texture::Type type) override;
  void DestroyTexture(Texture* texture) noexcept override;

  void Attach(SDL_Window* window);
  void Detach();
  void Destroy();

  void DrawImage(
      const RenderTransform& transform,
      const Point& origin,
      const Rect& box,
      const IntRect& src,
      Texture* texture,
      const RenderFilter& filter) noexcept override;

  void DrawImage(
      const Rect& box,
      const IntRect& src,
      Texture* texture,
      const RenderFilter& filter) noexcept override;

  void DrawImageCapInsets(
      const Rect& box,
      const EdgeRect& capInsets,
      Texture* texture,
      const RenderFilter& filter) noexcept override;

  void FillRect(
      const Rect& box,
      const RenderFilter& filter) noexcept override;

  void StrokeRect(
      const Rect& box,
      const EdgeRect& edges,
      const RenderFilter& filter) noexcept override;

 private:
  void ResetInternal();
  void SetRenderDrawColor(color_t color) noexcept;
  void UpdateTextureFormats(const SDL_RendererInfo& info) noexcept;

 private:
  SDL_Renderer* renderer{};
  phmap::flat_hash_set<Texture*> textures{};
  bool floatMode{false};
  PixelFormat textureFormat{PixelFormatUnknown};
  color_t drawColor{};
  Texture* fillRectTexture{};
  int32_t width{0};
  int32_t height{0};
};

} // namespace lse
