/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <ls/Log.h>

namespace ls {

template<typename T>
typename AsyncWorkState<T>::AsyncWorkStateMap AsyncWorkState<T>::sAsyncWorkStateMap;
template<typename T>
std::mutex AsyncWorkState<T>::sAsyncWorkStateMapMutex;

template<typename T>
AsyncWorkState<T>::AsyncWorkState(Napi::Env env, AsyncWorkCreate<T>&& create, AsyncWorkComplete<T>&& complete)
: env(env), create(std::move(create)), complete(std::move(complete)) {
    const auto executeCallback = [](napi_env env, void* data) {
      auto state{ GetState(static_cast<AsyncWorkState<T>*>(data)) };

      if (!state) {
          return;
      }

      try {
          state->result = AsyncWorkResult<T>(state->create());
      } catch (std::exception& e) {
          state->result = AsyncWorkResult<T>(e.what());
      }
    };

    const auto completeCallback = [](napi_env env, napi_status status, void* data) {
      constexpr auto LAMBDA_FUNCTION = "AsyncWorkComplete";
      auto state{ GetState(static_cast<AsyncWorkState<T>*>(data)) };

      if (!state) {
          LOG_WARN_LAMBDA("Unregistered work state");
          return;
      }

      if (status == napi_ok) {
          try {
              state->complete(env, &state->result);
          } catch (std::exception& e) {
              LOG_ERROR_LAMBDA("Callback error: %s", e);
          }
      } else if (status != napi_cancelled) {
          LOG_WARN_LAMBDA("Unexpected status: %i", status);
      }

      state->Destroy();
    };

    Napi::HandleScope scope(env);
    auto resource{ Napi::Object::New(env) };
    auto status{
        napi_create_async_work(env, resource, resource, executeCallback, completeCallback, this, &this->work)
    };

    NAPI_THROW_IF_FAILED_VOID(env, status);
}

template<typename T>
AsyncWorkState<T>::~AsyncWorkState() {
    this->Destroy();
}

template<typename T>
void AsyncWorkState<T>::Queue() {
    if (this->work && !this->IsQueued()) {
        this->isQueued = true;
        InsertState(this);

        auto status{ napi_queue_async_work(this->env, this->work) };

        if (status != napi_ok) {
            EraseState(this);
        }

        NAPI_THROW_IF_FAILED_VOID(this->env, status);
    }
}

template<typename T>
void AsyncWorkState<T>::Cancel() {
    if (this->work) {
        if (this->isQueued) {
            AsyncWorkState<T>::EraseState(this);

            auto status{ napi_cancel_async_work(this->env, this->work) };

            // napi_generic_failure means the work is inflight and cannot be destroyed until the
            // complete callback is invoked. The complete callback will destroy the work.

            if (status != napi_generic_failure) {
                NAPI_THROW_IF_FAILED_VOID(this->env, status);
            }
        } else {
            // Work has not been queued, so it is safe to release all resources.
            this->Destroy();
        }
    }
}

template<typename T>
void AsyncWorkState<T>::Destroy() {
    if (this->isQueued) {
        AsyncWorkState<T>::EraseState(this);
        this->isQueued = false;
    }

    if (this->work) {
        napi_delete_async_work(this->env, this->work);
        this->work = nullptr;
    }
}

template<typename T>
bool AsyncWorkState<T>::IsQueued() const {
    return isQueued;
}

template<typename T>
std::shared_ptr<AsyncWorkState<T>> AsyncWorkState<T>::GetState(const AsyncWorkState<T>* state) {
    std::lock_guard<std::mutex> guard(sAsyncWorkStateMapMutex);
    auto i{ sAsyncWorkStateMap.find(state) };

    if (i != sAsyncWorkStateMap.end()) {
        return i->second;
    }

    return nullptr;
}

template<typename T>
void AsyncWorkState<T>::EraseState(AsyncWorkState<T>* state) {
    std::lock_guard<std::mutex> guard(sAsyncWorkStateMapMutex);

    sAsyncWorkStateMap.erase(state);
}

template<typename T>
void AsyncWorkState<T>::InsertState(AsyncWorkState<T>* state) {
    std::lock_guard<std::mutex> guard(sAsyncWorkStateMapMutex);

    sAsyncWorkStateMap[state] = state->shared_from_this();
}

template<typename T>
void AsyncWork<T>::Reset(Napi::Env e, AsyncWorkCreate<T>&& create, AsyncWorkComplete<T>&& complete) {
    this->Cancel();
    this->env = e;
    this->state = std::make_shared<AsyncWorkState<T>>(e, std::move(create), std::move(complete));
}

template<typename T>
AsyncWork<T>::~AsyncWork() {
    this->Cancel();
}

template<typename T>
void AsyncWork<T>::Queue() {
    if (this->state) {
        this->state->Queue();
    }
}

template<typename T>
void AsyncWork<T>::Cancel() {
    if (this->state) {
        this->state->Cancel();
    }
}

} // namespace ls
