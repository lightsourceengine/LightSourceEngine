/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <functional>
#include <vector>
#include <utility>
#include <string>

namespace ls {

enum ResourceState : uint8_t {
    ResourceStateInit,
    ResourceStateReady,
    ResourceStateError,
    ResourceStateLoading,
    ResourceStateSyncing,
};

class ResourceManager;

class Resource {
 public:
    explicit Resource(const std::string& id);
    virtual ~Resource() = default;

    uint32_t AddListener(std::function<void()> listener);
    void RemoveListener(const uint32_t listenerId);

    int32_t AddRef() { return ++this->refCount; }
    int32_t RemoveRef() { return --this->refCount; }
    int32_t GetRefCount() const { return this->refCount; }

    const std::string& GetId() const { return this->id; }

 protected:
    void SetStateAndNotifyListeners(ResourceState newState);

 private:
    void RemoveListenerById(const uint32_t listenerId);

 protected:
    std::string id;
    ResourceState resourceState{ ResourceStateInit };

 private:
    static uint32_t nextListenerId;
    int32_t refCount{ 1 };
    std::vector<std::pair<uint32_t, std::function<void()>>> listeners{};
    uint8_t dispatchState{ 0 };

    friend ResourceManager;
};

} // namespace ls
