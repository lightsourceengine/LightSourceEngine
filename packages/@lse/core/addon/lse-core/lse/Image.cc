/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "Image.h"

#include <lse/Log.h>
#include <lse/DecodeImage.h>
#include <lse/Renderer.h>
#include <lse/string-ext.h>

namespace lse {

int32_t Image::nextResourceId{1};

Image::Image(const ImageRequest& request) noexcept : resourceId(nextResourceId++) {
  this->request = request;
}

Image::Image(int32_t width, int32_t height) noexcept
: resourceId(nextResourceId++), width(width), height(height) {
}

int32_t Image::GetId() const noexcept {
  return this->resourceId;
}

bool Image::HasDimensions() const noexcept {
  return this->width > 0 && this->height > 0;
}

int32_t Image::Width() const noexcept {
  return this->width;
}

int32_t Image::Height() const noexcept {
  return this->height;
}

float Image::WidthF() const noexcept {
  return this->width;
}

float Image::HeightF() const noexcept {
  return this->height;
}

bool Image::SafeIsReady(Image* image) noexcept {
  return image && image->IsReady();
}

float Image::AspectRatio() const noexcept {
  return this->height > 0 ? this->WidthF() / this->HeightF() : 0;
}

const std::string& Image::GetErrorMessage() const noexcept {
  if (this->IsError()) {
    return this->errorMessage;
  } else {
    static std::string emptyString{};
    return emptyString;
  }
}

void Image::AddListener(void* owner, Listener listener) {
  assert(owner != nullptr);
  assert(listener != nullptr);

  if (owner == nullptr || listener == nullptr) {
    return;
  }

  const auto hasOwner{
    std::find_if(
      this->listeners.cbegin(),
      this->listeners.cend(),
      [owner](const ListenerEntry& e) noexcept { return e.owner == owner; }) != this->listeners.cend()
  };

  assert(!hasOwner);

  if (hasOwner) {
    return;
  }

  this->listeners.emplace_back(ListenerEntry{ listener, owner });
}

void Image::RemoveListener(void* owner) {
  if (owner == nullptr) {
    return;
  }

  for (auto& entry : this->listeners) {
    if (owner == entry.owner) {
      entry = {};
    }
  }
}

void Image::NotifyListeners() {
  for (const auto& entry : this->listeners) {
    if (entry.owner) {
      entry.listener(entry.owner, this);
    }
  }

  this->listeners.erase(
      std::remove_if(
          this->listeners.begin(),
          this->listeners.end(),
          [](const ListenerEntry& e) { return e.owner == nullptr; }),
      this->listeners.end());
}

void Image::Attach(Renderer* renderer) {
  if (this->IsAttached()) {
    return;
  }

  this->renderer = renderer;
  this->rendererTextureFormat = renderer->GetTextureFormat();
}

void Image::Detach(Renderer* renderer) {
  if (!this->IsAttached()) {
    return;
  }

  this->renderer = nullptr;
  this->texture = Texture::SafeDestroy(this->texture);

  if (this->state == ImageState::Ready) {
    this->state = ImageState::Init;
  }
}

bool Image::IsAttached() const noexcept {
  return this->renderer != nullptr;
}

void Image::Destroy() {
  // TODO: clean up?
  this->isDestroyed = true;
}

void Image::OnLoadImageAsync() noexcept {
  try {
    if (StartsWith(this->request.uri, "data:")) {
      this->bytes = DecodeImageFromDataUri(this->request.uri, this->request.width, this->request.height);
    } else {
      this->bytes = DecodeImageFromFile(this->request.uri, this->request.width, this->request.height);
    }

    this->bytes.SyncFormat(this->rendererTextureFormat);
  } catch (const std::exception& e) {
    this->errorMessage = e.what();
    this->bytes = {};
  }
}

void Image::OnLoadImageAsyncComplete() noexcept {
  if (this->isDestroyed) {
    this->bytes.Release();
    this->errorMessage = "Image destroyed";
    this->state = ImageState::Error;
    return;
  }

  this->width = this->bytes.Width();
  this->height = this->bytes.Height();

  if (this->IsAttached()) {
    if (!this->bytes.Bytes()) {
      this->state = ImageState::Error;
    } else {
      this->bytes.SyncFormat(this->rendererTextureFormat);
      this->texture = this->renderer->CreateTexture(
          this->bytes.Width(), this->bytes.Height(), Texture::Updatable);

      if (this->texture && this->texture->Update(this->bytes.Bytes())) {
        this->state = ImageState::Ready;
      } else {
        this->texture = Texture::SafeDestroy(this->texture);
        this->errorMessage = "Failed to create texture";
        this->state = ImageState::Error;
      }
    }

    this->bytes.Release();
    this->NotifyListeners();
  } else {
    // TODO: wait for attach...
  }
}

const ImageRequest& Image::GetRequest() const noexcept {
  return this->request;
}

ImageState Image::GetState() const noexcept {
  return this->state;
}

bool Image::IsReady() const noexcept {
  return this->state == ImageState::Ready;
}

bool Image::IsError() const noexcept {
  return this->state == ImageState::Error;
}

bool Image::IsLoading() const noexcept {
  return this->state == ImageState::Loading;
}

Texture* Image::GetTexture() const noexcept {
  return this->texture;
}

bool operator==(const ImageRequest& lhs, const ImageRequest& rhs) noexcept {
  return lhs.uri == rhs.uri && lhs.width == rhs.width && lhs.height == rhs.height;
}

} // namespace lse
