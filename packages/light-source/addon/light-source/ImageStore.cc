/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "ImageStore.h"

namespace ls {

const std::vector<std::string> ImageStore::defaultExtensions{ ".jpg", ".jpeg", ".png", ".gif", ".svg" };

ImageStore::ImageStore() noexcept
: ResourceStore<ImageResource, ImageResource::idType>(), searchExtensions(defaultExtensions) {
}

void ImageStore::Attach(Scene* scene) noexcept {
    assert(!this->IsAttached());

    this->scene = scene;

    for (const auto& entry : this->resources) {
        entry.second->Attach(scene);
    }
}

void ImageStore::Detach() noexcept {
    if (!this->IsAttached()) {
        return;
    }

    for (const auto& entry : this->resources) {
        entry.second->Detach();
    }

    this->scene = nullptr;
}

std::shared_ptr<ImageResource> ImageStore::LoadImage(const ImageUri& uri) {
    auto image{ std::make_shared<ImageResource>(uri) };

    if (this->scene) {
        image->Attach(this->scene);
    }

    this->Add(image);

    return image;
}

std::shared_ptr<ImageResource> ImageStore::GetOrLoadImage(const ImageUri& uri) {
    return this->Has(uri.GetId()) ? this->Get(uri.GetId()) : this->LoadImage(uri);
}

std::shared_ptr<ImageResource> ImageStore::GetImage(const std::string& id) const {
    return this->Get({id});
}

bool ImageStore::HasImage(const std::string& id) const {
    return this->Has({id});
}

void ImageStore::ProcessEvents() {
    // TODO: process deletes
}

} // namespace ls
