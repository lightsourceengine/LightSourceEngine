/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <lse/RefRenderer.h>

namespace lse {

class RefTexture final : public Texture {
 public:
  RefTexture(std::shared_ptr<Renderer> renderer, int32_t width, int32_t height, PixelFormat format, Type type) noexcept
    : Texture(std::move(renderer), this, width, height, format, type) {
  }

  bool Update(const uint8_t* pixels) noexcept override {
    return true;
  }

  uint8_t* Lock() noexcept override {
    this->lockedPixels = new uint8_t[this->height * this->Pitch()];
    return this->lockedPixels;
  }

  void Unlock() noexcept override {
    delete [] this->lockedPixels;
    this->lockedPixels = nullptr;
  }

 private:
  uint8_t* lockedPixels{};
};

std::shared_ptr<RefRenderer> RefRenderer::New() {
  return std::make_shared<RefRenderer>();
}

int32_t RefRenderer::GetWidth() const noexcept {
  return 0;
}

int32_t RefRenderer::GetHeight() const noexcept {
  return 0;
}

void RefRenderer::EnabledClipping(const Rect& rect) noexcept {
}

void RefRenderer::DisableClipping() noexcept {
}

bool RefRenderer::SetRenderTarget(Texture* texture) noexcept {
  return true;
}

void RefRenderer::Reset() noexcept {
}

Texture* RefRenderer::CreateTexture(int32_t width, int32_t height, Texture::Type type) {
  return new RefTexture(this->shared_from_this(), width, height, this->GetTextureFormat(), type);
}

void RefRenderer::DestroyTexture(Texture* texture) {
  delete texture;
}

} // namespace lse
