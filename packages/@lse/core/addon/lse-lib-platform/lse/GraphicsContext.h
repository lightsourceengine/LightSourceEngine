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
#include <lse/Reference.h>
#include <string>

namespace lse {

struct GraphicsContextConfig {
  int32_t width{};
  int32_t height{};
  int32_t displayIndex{};
  bool fullscreen{};
  std::string fullscreenMode{};
};

/**
 * Interface for controlling the visual display of a Window (Screen).
 *
 * GraphicsContext is the interface the Platform Plugin uses to abstract the window (screen) and
 * renderer (OpenGL, DirectX, etc drawing). The Scene will use this interface to render the
 * scene graph and allow an app to change properties, like display resolution, etc.
 */
class GraphicsContext : public Reference {
 public:
  virtual ~GraphicsContext() noexcept = default;

  virtual void Attach() = 0;
  virtual void Detach() = 0;
  virtual void Resize();

  void SetConfig(const GraphicsContextConfig& config) noexcept;
  virtual void SetTitle(const char* title);
  virtual const char* GetTitle() const noexcept;

  int32_t GetWidth() const noexcept;
  int32_t GetHeight() const noexcept;
  Renderer* GetRenderer() const noexcept;
  bool IsFullscreen() const noexcept;
  int32_t GetDisplayIndex() const noexcept;
  int32_t GetRefreshRate() const noexcept;

 protected:
  void SetRenderer(std::shared_ptr<Renderer> value) noexcept;

 protected:
  std::string title{};
  GraphicsContextConfig config{};
  int32_t width{};
  int32_t height{};
  bool fullscreen{};
  int32_t displayIndex{};
  int32_t refreshRate{};

 private:
  std::shared_ptr<Renderer> renderer{};
};

using GraphicsContextRef = std::shared_ptr<GraphicsContext>;

} // namespace lse
