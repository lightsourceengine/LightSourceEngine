/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "Resource.h"
#include "Task.h"
#include "ImageUri.h"
#include <ls/Surface.h>

namespace ls {

class Scene;
class Renderer;
class ImageStore;
class Texture;

class ImageResource : public Resource<std::string> {
 public:
    explicit ImageResource(const ImageUri& uri) noexcept;
    virtual ~ImageResource() noexcept;

    std::shared_ptr<Texture> GetTexture() const noexcept { return this->texture; }
    bool HasTexture() const noexcept { return !!this->texture; }

    int32_t GetWidth() const noexcept { return this->width; }
    int32_t GetHeight() const noexcept { return this->height; }

    float GetWidthF() const noexcept { return static_cast<float>(this->width); }
    float GetHeightF() const noexcept { return static_cast<float>(this->height); }

    float GetAspectRatio() const noexcept { return this->aspectRatio; }
    bool HasCapInsets() const noexcept { return this->uri.HasCapInsets(); }
    const EdgeRect& GetCapInsets() const noexcept { return this->uri.GetCapInsets(); }
    const ImageUri& GetUri() const noexcept { return this->uri; }

    bool Sync(Renderer* renderer);

 private:
    void Load();
    void Attach(Scene* scene);
    void Detach();

 private:
    Scene* scene{};
    ImageUri uri{};
    int32_t width{};
    int32_t height{};
    float aspectRatio{0.f};
    std::shared_ptr<Texture> texture;
    Task loadImageTask;
    Surface image;

    friend ImageStore;
};

} // namespace ls
