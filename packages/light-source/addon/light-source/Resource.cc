/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Resource.h"
#include <fmt/format.h>

namespace ls {

uint32_t Resource::nextListenerId{1};
constexpr uint8_t DispatchStateDispatching = 1 << 0;
constexpr uint8_t DispatchStateHasRemovals = 1 << 1;

Resource::Resource(const std::string& id) : id(id) {
}

void Resource::SetState(ResourceState newState) {
    if (this->resourceState == newState) {
        return;
    }

    this->resourceState = newState;
}

void Resource::SetStateAndNotifyListeners(ResourceState newState) {
    this->SetState(newState);

    if (this->listeners.empty()) {
        return;
    }

    this->dispatchState = DispatchStateDispatching;

    auto listenerCount{ this->listeners.size() };

    for (decltype(listenerCount) i{0}; i < listenerCount; i++) {
        auto& p{ this->listeners[i] };

        if (p.first != 0) {
            try {
                this->listeners[i].second();
            } catch (std::exception& e) {
                fmt::println("Error: [resourceId={}] Resource listener exception {}", this->id, e.what());
            }
        }
    }

    if (this->dispatchState & DispatchStateHasRemovals) {
        this->RemoveListenerById(0);
    }

    this->dispatchState = 0;
}

uint32_t Resource::AddListener(std::function<void()> listener) {
    auto listenerId{ nextListenerId++ };

    this->listeners.push_back(std::make_pair(listenerId, listener));

    return listenerId;
}

void Resource::RemoveListener(const uint32_t listenerId) {
    if (this->dispatchState & DispatchStateDispatching) {
        for (auto& it : this->listeners) {
            if (it.first == listenerId) {
                this->dispatchState |= DispatchStateHasRemovals;
                it.first = 0;
                break;
            }
        }
    } else {
        this->RemoveListenerById(listenerId);
    }
}

void Resource::RemoveListenerById(const uint32_t listenerId) {
    auto removals{ std::remove_if(
        this->listeners.begin(),
        this->listeners.end(),
        [listenerId](std::pair<uint32_t, std::function<void()>> const & p) { return p.first == listenerId; }) };

    this->listeners.erase(removals, this->listeners.end());
}

std::string ResourceStateToString(ResourceState state) {
    switch (state) {
        case ResourceStateInit:
            return "init";
        case ResourceStateReady:
            return "ready";
        case ResourceStateError:
            return "error";
        case ResourceStateLoading:
            return "loading";
    }

    return "unknown";
}

} // namespace ls
