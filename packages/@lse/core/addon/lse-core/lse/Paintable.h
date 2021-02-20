/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <lse/Texture.h>

namespace lse {

class Renderer;
class RenderingContext2D;

/**
 * Abstract class for drawing vector graphic element backgrounds (text, border radius, etc) to a texture.
 */
class Paintable {
 public:
  virtual ~Paintable() noexcept = default;

  /**
   * Write paintable contents to the underlying texture.
   */
  virtual void Paint(RenderingContext2D* context) = 0;

  /**
   * Clean up texture resources used by this object.
   */
  void Destroy() noexcept;

  /**
   * Get the underlying texture object. If not initialized, the texture will be "null".
   */
  Texture* GetTexture() const noexcept;

 protected:
  /**
   * Ensure the underlying texture is width x height. If no texture exists, one will be created. If the texture
   * exists, but the size does not exactly match width x height, a new texture will be created.
   */
  Texture* EnsureLockableTexture(Renderer* renderer, int32_t width, int32_t height) noexcept;

 private:
  Texture* target{};
};

} // namespace lse
