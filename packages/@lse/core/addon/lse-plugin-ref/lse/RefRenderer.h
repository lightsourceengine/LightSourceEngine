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

#include <lse/Renderer.h>
#include <memory>

namespace lse {

class RefRenderer : public Renderer, public std::enable_shared_from_this<RefRenderer> {
 public:
  ~RefRenderer() override = default;

  static std::shared_ptr<RefRenderer> New();

  int32_t GetWidth() const noexcept override;
  int32_t GetHeight() const noexcept override;
  PixelFormat GetTextureFormat() const noexcept override { return PixelFormatRGBA; }

  bool SetRenderTarget(Texture* texture) noexcept override;
  void Reset() noexcept override;
  void EnabledClipping(const Rect& rect) noexcept override;
  void DisableClipping() noexcept override;

  Texture* CreateTexture(int32_t width, int32_t height, Texture::Type type) override;
  void DestroyTexture(Texture* texture) override;
};

} // namespace lse
