/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "RefSceneAdapter.h"
#include <fmt/format.h>

namespace ls {

RefSceneAdapter::RefSceneAdapter(const SceneAdapterConfig& config)
    : renderer(std::make_unique<RefRenderer>()), config(config) {
}

RefSceneAdapter::~RefSceneAdapter() {
}

void RefSceneAdapter::Attach() {
}

void RefSceneAdapter::Detach() {
}

void RefSceneAdapter::Resize(int32_t width, int32_t height, bool fullscreen) {
}

int32_t RefSceneAdapter::GetWidth() const {
    return this->config.width;
}

int32_t RefSceneAdapter::GetHeight() const {
    return this->config.height;
}

bool RefSceneAdapter::GetFullscreen() const {
    return this->config.fullscreen;
}

Renderer* RefSceneAdapter::GetRenderer() const {
    return this->renderer.get();
}

} // namespace ls
