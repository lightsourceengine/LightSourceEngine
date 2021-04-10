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
