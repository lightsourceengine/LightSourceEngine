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

#include "RefGraphicsContext.h"

#include <lse/RefRenderer.h>

namespace lse {

RefGraphicsContext::RefGraphicsContext(const GraphicsContextConfig& config) : GraphicsContext() {
  this->refRenderer = RefRenderer::New();
  this->SetRenderer(this->refRenderer);
  this->SetConfig(config);
}

void RefGraphicsContext::Attach() {
  this->width = this->config.width;
  this->height = this->config.height;
  this->displayIndex = this->config.displayIndex;
  this->fullscreen = this->config.fullscreen;
}

void RefGraphicsContext::Detach() {
  this->width = {};
  this->height = {};
  this->displayIndex = {};
  this->fullscreen = {};
}

} // namespace lse
