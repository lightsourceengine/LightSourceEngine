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

#include <lse/Rect.h>
#include <lse/Point.h>
#include <lse/Texture.h>
#include <lse/Matrix.h>
#include <lse/Color.h>
#include <lse/PixelFormat.h>
#include <cstdint>

namespace lse {

struct RenderTransform {
  float tx{};
  float ty{};
  float sx{1};
  float sy{1};
  float rotate{};

  bool HasRotate() const noexcept {
    return this->rotate < -0.99f || this->rotate > 0.99f;
  }
};

struct RenderFilter {
  color_t tint{ColorWhite};
  bool flipH{false};
  bool flipV{false};

  bool HasFlip() const noexcept {
    return this->flipH || this->flipV;
  }

  static RenderFilter OfTint(color_t tint, float opacity = 1.f) noexcept {
    return { tint.MixAlpha(opacity), {}, {} };
  }

  static RenderFilter OfFlip(bool flipH, bool flipV) noexcept {
    return { {}, flipH, flipV };
  }
};

/**
 * Interface for rendering to the screen and creating textures (images).
 */
class Renderer {
 public:
  virtual ~Renderer() = default;

  /**
   * Update the screen with any rendering performed since the previous call.
   *
   * If vsync is enabled, this method will block waiting for the next vertical blank.
   */
  virtual void Present() noexcept {};

  /**
   * Resets the state of the renderer.
   *
   * Clipping is disabled. The render target is reset to the screen.
   */
  virtual void Reset() noexcept = 0;

  /**
   * @return The width of the renderable area in pixels.
   */
  virtual int32_t GetWidth() const noexcept = 0;

  /**
   * @return The height of renderable area in pixels.
   */
  virtual int32_t GetHeight() const noexcept = 0;

  /**
   * Sets the target texture of the renderer.
   *
   * To reset the render target to the screen (default behavior), use Reset().
   *
   * @param renderTarget A texture of Type::RenderTarget.
   * @return true if the render target was successfully set, false if renderTarget is incompatible as a render target
   */
  virtual bool SetRenderTarget(Texture* texture) noexcept = 0;

  /**
   * Clip draw calls by a rectangular region.
   */
  virtual void EnabledClipping(const Rect& rect) noexcept = 0;

  /**
   * Turn off clipping.
   */
  virtual void DisableClipping() noexcept = 0;

  /**
   * Create a new texture.
   *
   * @param width Width, in pixels, of new texture.
   * @param height Height, in pixels, of new texture.
   * @param type The type of texture to create.
   * @return on success, a valid texture; on failure, nullptr
   */
  virtual Texture* CreateTexture(int32_t width, int32_t height, Texture::Type type) = 0;

  /**
   * Destroy a texture.
   *
   * @param texture Texture to destroy. no-op if null.
   */
  virtual void DestroyTexture(Texture* texture) = 0;

  /**
   * Get the pixel format for all new textures.
   */
  virtual PixelFormat GetTextureFormat() const noexcept = 0;

  /**
   * Fill the render target with the specified color.
   */
  virtual void Clear(color_t color) noexcept {};

  virtual void DrawImage(
      const RenderTransform& transform,
      const Point& origin,
      const Rect& box,
      const IntRect& src,
      Texture* texture,
      const RenderFilter& filter) noexcept {};

  virtual void DrawImage(
      const Rect& box,
      const IntRect& src,
      Texture* texture,
      const RenderFilter& filter) noexcept {};

  virtual void DrawImageCapInsets(
      const Rect& box,
      const EdgeRect& capInsets,
      Texture* texture,
      const RenderFilter& filter) noexcept {};

  virtual void FillRect(
      const Rect& box,
      const RenderFilter& filter) noexcept {};

  virtual void StrokeRect(
      const Rect& box,
      const EdgeRect& edges,
      const RenderFilter& filter) noexcept {};
};

} // namespace lse
