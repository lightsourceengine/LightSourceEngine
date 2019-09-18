/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>
#include "Stage.h"
#include "Scene.h"

namespace ls {

/**
 *
 */
template<typename ResourceType, typename IdType, typename IdHash = std::hash<IdType>>
class ResourceStore {
 public:
    explicit ResourceStore(Stage* stage, Scene* scene = nullptr);
    virtual ~ResourceStore();

    /**
     *
     */
    void Add(const std::shared_ptr<ResourceType> resource);

    /**
     *
     */
    void Remove(const IdType& id);

    /**
     *
     */
    bool Has(const IdType& id) const;

    /**
     *
     */
    std::shared_ptr<ResourceType> Get(const IdType& id) const;

    /**
     *
     */
    void Attach();

    /**
     *
     */
    void Detach();

    /**
     *
     */
    void ForEach(std::function<void(std::shared_ptr<ResourceType>)> func);

 protected:
    Stage* stage;
    Scene* scene;
    std::unordered_map<IdType, std::shared_ptr<ResourceType>, IdHash> resources;
};

template<typename ResourceType, typename IdType, typename IdHash>
ResourceStore<ResourceType, IdType, IdHash>::ResourceStore(Stage* stage, Scene* scene) : stage(stage), scene(scene) {
    this->stage->Ref();

    if (this->scene) {
        this->scene->Ref();
    }
}

template<typename ResourceType, typename IdType, typename IdHash>
ResourceStore<ResourceType, IdType, IdHash>::~ResourceStore() {
    if (this->scene) {
        this->scene->Unref();
    }

    this->stage->Unref();
}

template<typename ResourceType, typename IdType, typename IdHash>
bool ResourceStore<ResourceType, IdType, IdHash>::Has(const IdType& id) const {
    return this->resources.find(id) != this->resources.end();
}

template<typename ResourceType, typename IdType, typename IdHash>
std::shared_ptr<ResourceType> ResourceStore<ResourceType, IdType, IdHash>::Get(const IdType& id) const {
    auto it{ this->resources.find(id) };

    if (it != this->resources.end()) {
        return it->second;
    }

    return {};
}

template<typename ResourceType, typename IdType, typename IdHash>
void ResourceStore<ResourceType, IdType, IdHash>::Remove(const IdType& id) {
    auto it{ this->resources.find(id) };

    if (it != this->resources.end()) {
        it->second->Reset(this->stage, this->scene);
        this->resources.erase(it);
    }
}

template<typename ResourceType, typename IdType, typename IdHash>
void ResourceStore<ResourceType, IdType, IdHash>::Add(std::shared_ptr<ResourceType> resource) {
    assert(!this->Has(resource->GetId()));

    this->resources.insert(std::make_pair(resource->GetId(), resource));
}

template<typename ResourceType, typename IdType, typename IdHash>
void ResourceStore<ResourceType, IdType, IdHash>::Attach() {
    for (auto& p : this->resources) {
        p.second->Attach(this->stage, this->scene);
    }
}

template<typename ResourceType, typename IdType, typename IdHash>
void ResourceStore<ResourceType, IdType, IdHash>::Detach() {
    for (auto& p : this->resources) {
        p.second->Detach(this->stage, this->scene);
    }
}

template<typename ResourceType, typename IdType, typename IdHash>
void ResourceStore<ResourceType, IdType, IdHash>::ForEach(
        std::function<void(std::shared_ptr<ResourceType>)> func) {
    for (auto& p : this->resources) {
        func(p.second);
    }
}

} // namespace ls
