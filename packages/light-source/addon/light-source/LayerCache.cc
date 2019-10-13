/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "LayerCache.h"
#include "Layer.h"
#include <algorithm>
#include <cassert>

namespace ls {

void LayerCache::Attach(Scene* scene) {
    this->scene = scene;
    std::for_each(this->layers.begin(), this->layers.end(), [scene](Layer* layer){ layer->Attach(scene); });
}

void LayerCache::Detach() {
    std::for_each(this->layers.begin(), this->layers.end(), [](Layer* layer){ layer->Detach(); });
}

void LayerCache::Add(Layer* layer) {
    assert(layer != nullptr);
    assert(this->layers.find(layer) == this->layers.end());

    this->layers.insert(layer);

    if (scene) {
        layer->Attach(scene);
    }
}

void LayerCache::Remove(Layer* layer) {
    assert(layer != nullptr);

    auto it{ this->layers.find(layer) };

    if (it != this->layers.end()) {
        this->layers.erase(it);
    }
}

} // namespace ls
