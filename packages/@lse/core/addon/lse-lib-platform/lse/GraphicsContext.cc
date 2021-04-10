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

int32_t GraphicsContext::GetRefreshRate() const noexcept {
  return this->refreshRate;
}

void GraphicsContext::SetRenderer(std::shared_ptr<Renderer> value) noexcept {
  this->renderer = std::move(value);
}

} // namespace lse
