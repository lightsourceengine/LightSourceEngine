/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <functional>
#include <phmap.h>
#include <lse/Reference.h>
#include <lse/Image.h>

namespace lse {

class ImageManager : public Reference {
 public:
  using LoadImageAsync = std::function<void(Image*)>;

 public:
  ImageManager(LoadImageAsync&& loadImageAsync) noexcept;
  ~ImageManager() override = default;

  void Attach(Renderer* renderer) noexcept;
  void Detach() noexcept;
  void Destroy() noexcept;

  Image* Acquire(const ImageRequest& request);
  void Release(Image* image);

  static Image* SafeAcquire(ImageManager* imageManager,
                            const ImageRequest& request,
                            void* owner,
                            Image::Listener listener) noexcept;

  static Image* SafeRelease(ImageManager* imageManager,
                            Image* image,
                            void* owner) noexcept;

 private:
  LoadImageAsync loadImageAsync{};
  phmap::flat_hash_map<int32_t, Image*> imagesById{};
  phmap::flat_hash_map<std::string, Image*> imagesByUri{};
  bool isAttached{};
  bool isDestroyed{};
  Renderer* renderer{};
};

} // namespace lse
