/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <lse/Paintable.h>

#include <lse/Renderer.h>

namespace lse {

void Paintable::Destroy() noexcept {
  this->target = Texture::SafeDestroy(this->target);
}

Texture* Paintable::GetTexture() const noexcept {
  return this->target;
}

Texture* Paintable::EnsureLockableTexture(Renderer* renderer, int32_t width, int32_t height) noexcept {
  if (!this->target || !this->target->IsLockable()
      || this->target->Width() != width || this->target->Height() != height) {
    this->target = Texture::SafeDestroy(this->target);
    // TODO: limit size
    this->target = renderer->CreateTexture(width, height, Texture::Lockable);
  }

  return target;
}

} // namespace lse
