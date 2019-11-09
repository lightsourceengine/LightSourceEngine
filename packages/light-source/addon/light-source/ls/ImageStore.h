/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ResourceStore.h"
#include "ImageResource.h"

namespace ls {

class Scene;

class ImageStore : public ResourceStore<ImageResource, ImageResource::idType> {
 public:
    ImageStore() noexcept;

    void Attach(Scene* scene) noexcept;
    void Detach() noexcept;
    bool IsAttached() noexcept { return this->scene != nullptr; }

    std::shared_ptr<ImageResource> LoadImage(const ImageUri& uri);
    std::shared_ptr<ImageResource> GetImage(const std::string& id) const;
    std::shared_ptr<ImageResource> GetOrLoadImage(const ImageUri& uri);
    bool HasImage(const std::string& id) const;

    const std::vector<std::string>& GetSearchExtensions() const noexcept { return this->searchExtensions; }
    void SetSearchExtensions(const std::vector<std::string>& extensions) { this->searchExtensions = extensions; }

    void ProcessEvents();

 public:
    static const std::vector<std::string> defaultExtensions;

 private:
    std::vector<std::string> searchExtensions;
    Scene* scene{nullptr};
};

} // namespace ls
