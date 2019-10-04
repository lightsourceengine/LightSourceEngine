/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <functional>
#include <vector>
#include <utility>
#include <string>
#include <fmt/println.h>

namespace ls {

class Stage;
class Scene;

enum ResourceState : uint8_t {
    ResourceStateInit,
    ResourceStateReady,
    ResourceStateError,
    ResourceStateLoading,
};

enum DispatchState : uint8_t {
    DispatchStateIdle = 0,
    DispatchStateDispatching = 1 << 0,
    DispatchStateHasRemovals = 1 << 1,
};

std::string ResourceStateToString(ResourceState state);

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

    bool IsReady() const { return this->resourceState == ResourceStateReady; }
    bool HasError() const { return this->resourceState == ResourceStateError; }

 protected:
    void SetState(ResourceState newState);
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

/**
 *
 */
template<typename IdType>
class BaseResource {
 public:
    typedef std::function<void(BaseResource*)> ResourceListener;

 public:
    explicit BaseResource(const IdType& id) : id(id) {}
    virtual ~BaseResource() = default;

    /**
     *
     */
    uint32_t AddListener(ResourceListener listener);

    /**
     *
     */
    void RemoveListener(const uint32_t listenerId);

    /**
     *
     */
    const IdType GetId() const { return this->id; }

    /**
     *
     */
    bool IsReady() const { return this->resourceState == ResourceStateReady; }

    /**
     *
     */
    bool HasError() const { return this->resourceState == ResourceStateError; }

    /**
     *
     */
    void SetState(ResourceState newState, bool notifyListeners = false);

    /**
     *
     */
    virtual void Attach(Stage* stage, Scene* scene) {}

    /**
     *
     */
    virtual void Detach(Stage* stage, Scene* scene) {}

    /**
     *
     */
    virtual void Reset(Stage* stage, Scene* scene) {}

    /**
     *
     */
    ResourceState GetState() const { return this->resourceState; }

 protected:
    IdType id;
    std::vector<std::pair<uint32_t, ResourceListener>> listeners;
    uint32_t nextListenerId{ 1 };
    ResourceState resourceState{ ResourceStateInit };
    uint8_t dispatchState{ DispatchStateIdle };
};

template<typename IdType>
uint32_t BaseResource<IdType>::AddListener(ResourceListener listener) {
    auto listenerId{ nextListenerId++ };

    this->listeners.push_back(std::make_pair(listenerId, listener));

    return listenerId;
}

template<typename IdType>
void BaseResource<IdType>::RemoveListener(const uint32_t listenerId) {
    if (this->dispatchState & DispatchStateDispatching) {
        for (auto& it : this->listeners) {
            if (it.first == listenerId) {
                this->dispatchState |= DispatchStateHasRemovals;
                it.first = 0;
                break;
            }
        }
    } else {
        auto removals{
            std::remove_if(
                this->listeners.begin(),
                this->listeners.end(),
                [listenerId](std::pair<uint32_t, ResourceListener> const & p) {
                    return p.first == listenerId;
                })
        };

        this->listeners.erase(removals, this->listeners.end());
    }
}

template<typename IdType>
void BaseResource<IdType>::SetState(ResourceState newState, bool notifyListeners) {
    this->resourceState = newState;

    if (!notifyListeners || this->listeners.empty()) {
        return;
    }

    this->dispatchState = DispatchStateDispatching;

    auto listenerCount{ this->listeners.size() };

    for (decltype(listenerCount) i{0}; i < listenerCount; i++) {
        auto& p{ this->listeners[i] };

        if (p.first != 0) {
            try {
                this->listeners[i].second(this);
            } catch (std::exception& e) {
                 fmt::println("Error: [resourceId={}] Resource listener exception {}", this->id.family, e.what());
            }
        }
    }

    if (this->dispatchState & DispatchStateHasRemovals) {
        auto removals{ std::remove_if(
            this->listeners.begin(),
            this->listeners.end(),
            [](std::pair<uint32_t, ResourceListener> const & p) { return p.first == 0; }) };

        this->listeners.erase(removals, this->listeners.end());
    }

    this->dispatchState = DispatchStateIdle;
}

} // namespace ls
