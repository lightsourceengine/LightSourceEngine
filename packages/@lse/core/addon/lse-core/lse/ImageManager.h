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
