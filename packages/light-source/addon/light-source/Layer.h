/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <memory>

namespace ls {

class LayerCache;
class Scene;
class Texture;

class Layer {
 public:
    explicit Layer(LayerCache* layerCache);
    Layer(LayerCache* layerCache, const int32_t width, const int32_t height);
    virtual ~Layer();

    std::shared_ptr<Texture> Sync(const int32_t width, const int32_t height) noexcept;
    std::shared_ptr<Texture> GetTexture() const noexcept { return this->texture; }
    bool HasTexture() const noexcept { return !!this->texture; }
    bool IsDirty() const noexcept { return this->isDirty; }
    void MarkDirty() noexcept { this->isDirty = true; }

 private:
    void Attach(Scene* scene) noexcept;
    void Detach() noexcept;

 private:
    LayerCache* layerCache{};
    int32_t width{};
    int32_t height{};
    std::shared_ptr<Texture> texture{};
    Scene* scene{};
    bool isDirty{true};

    friend LayerCache;
};

} // namespace ls
