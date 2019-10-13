/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>

namespace ls {

/**
 *
 */
template<typename ResourceType, typename IdType, typename IdHash = std::hash<IdType>>
class ResourceStore {
 public:
    virtual ~ResourceStore() noexcept = default;

    /**
     *
     */
    bool Has(const IdType& id) const noexcept;

    /**
     *
     */
    std::shared_ptr<ResourceType> Get(const IdType& id) const noexcept;

    /**
     *
     */
    template<typename Callback>
    void ForEach(Callback callback) {
        for (auto& p : this->resources) {
            callback(p.second);
        }
    }

 protected:
    /**
     *
     */
    void Add(const std::shared_ptr<ResourceType> resource);

    /**
     *
     */
    void Remove(const IdType& id);

 protected:
    std::unordered_map<IdType, std::shared_ptr<ResourceType>, IdHash> resources;
};

template<typename ResourceType, typename IdType, typename IdHash>
bool ResourceStore<ResourceType, IdType, IdHash>::Has(const IdType& id) const noexcept {
    return this->resources.find(id) != this->resources.end();
}

template<typename ResourceType, typename IdType, typename IdHash>
std::shared_ptr<ResourceType> ResourceStore<ResourceType, IdType, IdHash>::Get(const IdType& id) const noexcept {
    auto it{ this->resources.find(id) };

    if (it != this->resources.end()) {
        return it->second;
    }

    return nullptr;
}

template<typename ResourceType, typename IdType, typename IdHash>
void ResourceStore<ResourceType, IdType, IdHash>::Remove(const IdType& id) {
    auto it{ this->resources.find(id) };

    if (it != this->resources.end()) {
        // TODO: notify resource
        // it->second->Reset(this->stage, this->scene);
        this->resources.erase(it);
    }
}

template<typename ResourceType, typename IdType, typename IdHash>
void ResourceStore<ResourceType, IdType, IdHash>::Add(std::shared_ptr<ResourceType> resource) {
    assert(!this->Has(resource->GetId()));

    this->resources.insert(std::make_pair(resource->GetId(), resource));
}

} // namespace ls
