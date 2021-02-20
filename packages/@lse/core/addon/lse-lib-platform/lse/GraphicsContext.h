/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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

 protected:
  void SetRenderer(std::shared_ptr<Renderer> value) noexcept;

 protected:
  std::string title{};
  GraphicsContextConfig config{};
  int32_t width{};
  int32_t height{};
  bool fullscreen{};
  int32_t displayIndex{};

 private:
  std::shared_ptr<Renderer> renderer{};
};

using GraphicsContextRef = std::shared_ptr<GraphicsContext>;

} // namespace lse
