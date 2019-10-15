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
#include <algorithm>
#include <ls/Log.h>

namespace ls {

enum ResourceState : uint8_t {
    ResourceStateInit,
    ResourceStateReady,
    ResourceStateError,
    ResourceStateLoading,
};

std::string ResourceStateToString(ResourceState state);

using ResourceStateChangeFunction = std::function<void()>;

/**
 *
 */
template<typename IdType>
class Resource {
 public:
    using idType = IdType;

    explicit Resource(const IdType& id) : id(id) {}
    virtual ~Resource() = default;

    uint32_t AddListener(ResourceStateChangeFunction&& listener);
    void RemoveListener(const uint32_t listenerId);
    const IdType& GetId() const noexcept { return this->id; }
    bool IsReady() const noexcept { return this->resourceState == ResourceStateReady; }
    bool HasError() const noexcept { return this->resourceState == ResourceStateError; }
    void SetState(ResourceState newState, bool notifyListeners = false);
    void DispatchState();
    ResourceState GetState() const noexcept { return this->resourceState; }

 protected:
    enum DispatchState : uint8_t {
        DispatchStateIdle = 0,
        DispatchStateDispatching = 1 << 0,
        DispatchStateHasRemovals = 1 << 1,
    };

    IdType id;
    std::vector<std::pair<uint32_t, ResourceStateChangeFunction>> listeners;
    static uint32_t nextListenerId;
    ResourceState resourceState{ ResourceStateInit };
    uint8_t dispatchState{ DispatchStateIdle };
};

template<typename R>
class ResourceLink {
 public:
    R* Get() const noexcept;
    void Listen(ResourceStateChangeFunction&& listener);
    void Unlisten();

    ResourceLink<R>& operator=(const std::shared_ptr<R>& p) noexcept;
    ResourceLink<R>& operator=(nullptr_t) noexcept;
    R* operator->() const noexcept;
    explicit operator bool() const noexcept;

 private:
    std::shared_ptr<R> resource;
    uint32_t listenerId{0};
};

template<typename IdType>
uint32_t Resource<IdType>::nextListenerId{1};

template<typename IdType>
uint32_t Resource<IdType>::AddListener(ResourceStateChangeFunction&& listener) {
    const auto listenerId{ nextListenerId++ };

    this->listeners.push_back(std::make_pair(listenerId, listener));

    return listenerId;
}

template<typename IdType>
void Resource<IdType>::RemoveListener(const uint32_t listenerId) {
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
                [listenerId](const std::pair<uint32_t, ResourceStateChangeFunction>& p) {
                    return p.first == listenerId;
                })
        };

        this->listeners.erase(removals, this->listeners.end());
    }
}

template<typename IdType>
void Resource<IdType>::SetState(ResourceState newState, bool notifyListeners) {
    this->resourceState = newState;

    if (notifyListeners && !this->listeners.empty()) {
        this->DispatchState();
    }
}

template<typename IdType>
void Resource<IdType>::DispatchState() {
    this->dispatchState = DispatchStateDispatching;

    auto listenerCount{ this->listeners.size() };

    for (decltype(listenerCount) i{0}; i < listenerCount; i++) {
        auto& p{ this->listeners[i] };

        if (p.first != 0) {
            try {
                this->listeners[i].second();
            } catch (std::exception& e) {
                LOG_ERROR("%s: %s", static_cast<std::string>(this->id), e);
            }
        }
    }

    if (this->dispatchState & DispatchStateHasRemovals) {
        auto removals{ std::remove_if(
            this->listeners.begin(),
            this->listeners.end(),
            [](const std::pair<uint32_t, ResourceStateChangeFunction>& p) {
                return p.first == 0;
            })
        };

        this->listeners.erase(removals, this->listeners.end());
    }

    this->dispatchState = DispatchStateIdle;
}

template<typename R>
R* ResourceLink<R>::Get() const noexcept {
    return this->resource.get();
}

template<typename R>
void ResourceLink<R>::Listen(ResourceStateChangeFunction&& listener) {
    if (this->resource) {
        this->Unlisten();
        this->listenerId = this->resource->AddListener(std::move(listener));
    }
}

template<typename R>
void ResourceLink<R>::Unlisten() {
    if (this->resource && this->listenerId) {
        this->resource->RemoveListener(this->listenerId);
        this->listenerId = 0;
    }
}

template<typename R>
ResourceLink<R>& ResourceLink<R>::operator=(const std::shared_ptr<R>& p) noexcept {
    this->Unlisten();
    this->resource = p;

    return *this;
}

template<typename R>
ResourceLink<R>& ResourceLink<R>::operator=(nullptr_t) noexcept {
    this->Unlisten();
    this->resource.reset();

    return *this;
}

template<typename R>
R* ResourceLink<R>::operator->() const noexcept {
    return this->resource.get();
}

template<typename R>
ResourceLink<R>::operator bool() const noexcept {
    return !!this->resource;
}

template<typename R>
inline bool operator==(const ResourceLink<R>& link, R* p) noexcept {
    return link ? link.Get() == p : nullptr == p;
}

template<typename R>
inline bool operator!=(const ResourceLink<R>& link, R* p) noexcept {
    return link ? link.Get() != p : nullptr != p;
}

} // namespace ls
