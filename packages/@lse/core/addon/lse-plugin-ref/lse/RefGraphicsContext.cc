/*
 * Copyright (C) 2021 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
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
