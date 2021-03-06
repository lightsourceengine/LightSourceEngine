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

#include <string>
#include <vector>
#include <cstdint>
#include <atomic>
#include <lse/Reference.h>
#include <lse/ImageBytes.h>

namespace lse {

class Texture;
class Renderer;

struct ImageRequest {
  std::string uri{};
  int32_t width{};
  int32_t height{};
};

enum class ImageState {
  Init = 0,
  Loading,
  Ready,
  Error,
};

class Image : public Reference {
 public:
  using Listener = void(*)(void*, Image*);

 public:
  Image() noexcept = default;
  Image(const ImageRequest& request) noexcept;
  Image(int32_t width, int32_t height) noexcept;

  int32_t GetId() const noexcept;

  const ImageRequest& GetRequest() const noexcept;

  ImageState GetState() const noexcept;
  bool IsReady() const noexcept;
  bool IsError() const noexcept;
  bool IsLoading() const noexcept;

  Texture* GetTexture() const noexcept;

  bool HasDimensions() const noexcept;
  int32_t Width() const noexcept;
  int32_t Height() const noexcept;
  float WidthF() const noexcept;
  float HeightF() const noexcept;
  float AspectRatio() const noexcept;
  const std::string& GetErrorMessage() const noexcept;

  void AddListener(void* owner, Listener listener);
  void RemoveListener(void* owner);

  void Attach(Renderer* renderer);
  void Detach(Renderer* renderer);
  bool IsAttached() const noexcept;
  void Destroy();

  void OnLoadImageAsync() noexcept;
  void OnLoadImageAsyncComplete() noexcept;

  static bool SafeIsReady(Image* image) noexcept;

 private:
  struct ListenerEntry {
    Listener listener{};
    void* owner{};
  };

  void NotifyListeners();

 private:
  static int32_t nextResourceId;
  int32_t resourceId{};
  ImageRequest request{};
  int32_t width{};
  int32_t height{};
  Texture* texture{};
  ImageState state{ImageState::Init};
  std::vector<ListenerEntry> listeners{};
  ImageBytes bytes{};
  Renderer* renderer{};
  bool isDestroyed{};
  std::string errorMessage{};
  std::atomic<PixelFormat> rendererTextureFormat{PixelFormatUnknown};
};

bool operator==(const ImageRequest& lhs, const ImageRequest& rhs) noexcept;

} // namespace lse
