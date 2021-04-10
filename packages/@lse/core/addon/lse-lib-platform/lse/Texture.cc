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

#include "Texture.h"

#include <cstring>
#include <lse/Renderer.h>

namespace lse {

Texture::Texture(
    std::shared_ptr<Renderer> owner,
    void* texture,
    int32_t width,
    int32_t height,
    PixelFormat format,
    Type type) noexcept
: owner(std::move(owner)), platformTexture(texture), width(width), height(height), format(format), type(type) {
}

int32_t Texture::Width() const noexcept {
  return this->width;
}

int32_t Texture::Height() const noexcept {
  return this->height;
}

int32_t Texture::Pitch() const noexcept {
  return this->width * 4;
}

bool Texture::IsRenderTarget() const noexcept {
  return this->type == Texture::RenderTarget;
}

bool Texture::IsLockable() const noexcept {
  return this->type == Texture::Lockable || this->type == Texture::RenderTarget;
}

bool Texture::IsUpdatable() const noexcept {
  return this->type == Texture::Updatable || this->type == Texture::RenderTarget;
}

PixelFormat Texture::Format() const noexcept {
  return this->format;
}

void Texture::Destroy() noexcept {
  if (this->owner) {
    this->owner->DestroyTexture(this);
  }
}

Texture* Texture::SafeDestroy(Texture* texture) noexcept {
  if (texture) {
    texture->Destroy();
  }
  return nullptr;
}

TextureLock::TextureLock(Texture* texture, bool clear) {
  if (texture) {
    this->pixels = texture->Lock();
    this->texture = texture;

    if (clear) {
      memset(pixels, 0, texture->Pitch() * texture->Height());
    }
  }
}

TextureLock::~TextureLock() {
  if (this->texture) {
    this->texture->Unlock();
  }
}

bool TextureLock::IsLocked() const noexcept {
  return this->pixels != nullptr;
}

uint8_t* TextureLock::GetPixels() const noexcept {
  return this->pixels;
}

int32_t TextureLock::GetWidth() const noexcept {
  return this->texture ? this->texture->Width() : 0;
}

int32_t TextureLock::GetHeight() const noexcept {
  return this->texture ? this->texture->Height() : 0;
}

} // namespace lse
