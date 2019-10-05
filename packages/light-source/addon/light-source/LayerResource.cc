/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "LayerResource.h"
#include <ls/Renderer.h>

namespace ls {

LayerResource::LayerResource(Renderer* renderer) : renderer(renderer) {
}

bool LayerResource::Sync(const int32_t width, const int32_t height) {
    if (this->textureId) {
        if (width <= this->width && height <= this->height) {
            this->isDirty = false;
            return true;
        }

        this->Release();
    }

    this->textureId = this->renderer->CreateTexture(width, height);

    if (this->textureId) {
        this->width = width;
        this->height = height;
        this->hasError = false;
    } else {
        this->width = this->height = 0;
        this->hasError = true;
    }

    this->isDirty = false;

    return this->textureId > 0;
}

void LayerResource::Release() {
    if (this->textureId) {
        this->renderer->DestroyTexture(this->textureId);
        this->textureId = 0;
        this->width = this->height = 0;
        this->hasError = false;
        this->isDirty = true;
    }
}

} // namespace ls
