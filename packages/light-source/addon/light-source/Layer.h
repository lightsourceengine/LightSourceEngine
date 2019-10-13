/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

namespace ls {

class LayerCache;
class Scene;

class Layer {
 public:
    explicit Layer(LayerCache* layerCache);
    Layer(LayerCache* layerCache, const int32_t width, const int32_t height);
    virtual ~Layer();

    uint32_t Sync(const int32_t width, const int32_t height) noexcept;
    uint32_t GetTexture() const noexcept { return this->textureId; }
    bool HasTexture() const noexcept { return this->textureId > 0; }
    bool IsDirty() const noexcept { return this->isDirty; }
    void MarkDirty() noexcept { this->isDirty = true; }

 private:
    void Attach(Scene* scene) noexcept;
    void Detach() noexcept;

 private:
    LayerCache* layerCache{};
    int32_t width{};
    int32_t height{};
    uint32_t textureId{};
    Scene* scene{};
    bool isDirty{true};

    friend LayerCache;
};

} // namespace ls
