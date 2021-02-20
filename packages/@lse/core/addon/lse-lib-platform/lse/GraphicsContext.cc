/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include "GraphicsContext.h"

namespace lse {

void GraphicsContext::Resize() {
}

void GraphicsContext::SetConfig(const GraphicsContextConfig& config) noexcept {
  this->config = config;
}

void GraphicsContext::SetTitle(const char* title) {
  this->title = title ? title : "";
}

const char* GraphicsContext::GetTitle() const noexcept {
  return this->title.c_str();
}

int32_t GraphicsContext::GetWidth() const noexcept {
  return this->width;
}

int32_t GraphicsContext::GetHeight() const noexcept {
  return this->height;
}

Renderer* GraphicsContext::GetRenderer() const noexcept {
  return this->renderer.get();
}

bool GraphicsContext::IsFullscreen() const noexcept {
  return this->fullscreen;
}

int32_t GraphicsContext::GetDisplayIndex() const noexcept {
  return this->displayIndex;
}

void GraphicsContext::SetRenderer(std::shared_ptr<Renderer> value) noexcept {
  this->renderer = std::move(value);
}

} // namespace lse
