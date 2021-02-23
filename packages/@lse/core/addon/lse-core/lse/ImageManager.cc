/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "ImageManager.h"

#include <lse/Image.h>

namespace lse {

ImageManager::ImageManager(LoadImageAsync&& loadImageAsync) noexcept
: loadImageAsync(std::move(loadImageAsync)) {
}

void ImageManager::Attach(Renderer* renderer) noexcept {
  if (this->isAttached) {
    return;
  }

  for (auto& entry : this->imagesById) {
    auto image{entry.second};

    image->Attach(renderer);

    if (image->GetState() == ImageState::Init) {
      this->loadImageAsync(image);
    }
  }

  this->renderer = renderer;
  this->isAttached = true;
}

void ImageManager::Detach(Renderer* renderer) noexcept {
  if (!this->isAttached) {
    return;
  }

  for (auto& entry : this->imagesById) {
    entry.second->Detach(renderer);
  }

  this->renderer = nullptr;
  this->isAttached = false;
}

void ImageManager::Destroy() noexcept {
  if (this->isDestroyed) {
    return;
  }

  for (auto& entry : this->imagesById) {
    auto image{entry.second};

    image->Destroy();
    image->Unref();
  }

  this->imagesById.clear();
  this->isDestroyed = true;
}

Image* ImageManager::Acquire(const ImageRequest& request) {
  if (this->isDestroyed) {
    return {};
  }

  // TODO: find by uri

  auto image{new Image(request)};

  if (this->renderer) {
    image->Attach(this->renderer);
  }

  image->Ref();

  this->loadImageAsync(image);
  this->imagesById[image->GetId()] = image;

  return image;
}

void ImageManager::Release(Image* image) {
  if (this->isDestroyed) {
    return;
  }

  if (image) {
    image->Unref();

    if (image->RefCount() == 1) {
      // TODO: remove here?
    }
  }
}

Image* ImageManager::SafeAcquire(ImageManager* imageManager, const ImageRequest& request,
    void* owner, Image::Listener listener) noexcept {
  auto image{imageManager->Acquire(request)};

  if (image) {
    switch (image->GetState()) {
      case ImageState::Ready:
      case ImageState::Error:
        listener(owner, image);
        break;
      default /* ImageState::Loading */:
        image->AddListener(owner, listener);
        break;
    }
  }

  return image;
}

Image* ImageManager::SafeRelease(ImageManager* imageManager, Image* image, void* owner) noexcept {
  if (image) {
    image->RemoveListener(owner);
    imageManager->Release(image);
  }

  return {};
}

} // namespace lse