/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <unordered_set>

namespace ls {

class Layer;
class Scene;

class LayerCache {
 public:
    void Attach(Scene* scene);
    void Detach();

 private:
    void Add(Layer* layer);
    void Remove(Layer* layer);

 private:
    Scene* scene{};
    std::unordered_set<Layer*> layers;

    friend Layer;
};

} // namespace ls
