/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

namespace ls {

class Renderer;

class LayerResource {
 public:
    explicit LayerResource(Renderer* renderer);
    virtual ~LayerResource() = default;

     bool IsReady() const { return !this->hasError && this->textureId > 0; }
     bool HasError() const { return this->hasError; }

     bool Sync(const int32_t width, const int32_t height);
     void Release();

     int32_t Width() const { return this->width; }
     int32_t Height() const { return this->height; }
     uint32_t TextureId() const { return this->textureId; }

 private:
    Renderer* renderer{nullptr};
    uint32_t textureId{0};
    int32_t width{0};
    int32_t height{0};
    bool hasError{false};
};

} // namespace ls
