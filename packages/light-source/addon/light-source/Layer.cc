/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Layer.h"
#include "Scene.h"
#include <ls/Renderer.h>

namespace ls {

Layer::Layer(LayerCache* layerCache) : layerCache(layerCache) {
    layerCache->Add(this);
}

Layer::Layer(LayerCache* layerCache, const int32_t width, const int32_t height)
: layerCache(layerCache), width(width), height(height) {
    layerCache->Add(this);
}

Layer::~Layer() {
    this->layerCache->Remove(this);
}

std::shared_ptr<Texture> Layer::Sync(const int32_t width, const int32_t height) noexcept {
    // TODO: limit size to screen
    if (!this->isDirty && this->HasTexture() && this->width == width && this->height == height) {
        return this->GetTexture();
    }

    this->isDirty = false;
    this->texture = this->scene->GetRenderer()->CreateTexture(width, height);

    if (!this->texture) {
        this->width = this->height = 0;

        return 0;
    }

    this->width = width;
    this->height = height;

    return this->texture;
}

void Layer::Attach(Scene* scene) noexcept {
    this->scene = scene;
}

void Layer::Detach() noexcept {
    if (!this->scene) {
        return;
    }

    if (this->texture) {
        this->texture = nullptr;
        this->isDirty = true;
    }

    this->scene = nullptr;
}

} // namespace ls
