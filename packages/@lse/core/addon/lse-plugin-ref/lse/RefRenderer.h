/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <lse/Renderer.h>

namespace lse {

class RefRenderer : public Renderer {
 public:
  ~RefRenderer() override = default;

  int32_t GetWidth() const noexcept override;
  int32_t GetHeight() const noexcept override;
  PixelFormat GetTextureFormat() const noexcept override { return PixelFormatRGBA; }

  bool SetRenderTarget(const Texture& newRenderTarget) noexcept override;
  void Reset() noexcept override;
  void EnabledClipping(const Rect& rect) noexcept override;
  void DisableClipping() noexcept override;

  Texture CreateTexture(int32_t width, int32_t height, Texture::Type type) override;
};

} // namespace lse
