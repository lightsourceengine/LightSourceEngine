/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#pragma once

#include <napi.h>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace ls {

/**
 * Represents the result of the AsyncWork create function. Used as an argument to the complete function.
 *
 * @tparam T type of object that the AsyncWork create function returns
 */
template<typename T>
class AsyncWorkResult {
 public:
    AsyncWorkResult() = default;
    AsyncWorkResult(const char* e) : error(e) {}
    AsyncWorkResult(T&& val) : value(std::move(val)) {}

    // If true, the create function threw an exception and error contains the exception's error message.
    bool HasError() const noexcept { return !this->error.empty(); }

    // Get an rvalue reference to the value the create function returned (if create succeeded).
    T&& TakeValue() noexcept { return std::move(this->value); }

    // Get an rvalue reference to the exception message thrown by the create function (if an error occurred).
    std::string&& TakeError() noexcept { return std::move(this->error); }

 private:
    T value{};
    std::string error{};
};

/**
 * The create function format used by AsyncWork.
 *
 * Create runs in a separate thread, so it may not access the javascript environment. If an error occurs, the function
 * should throw an std::exception.
 */
template<typename T>
using AsyncWorkCreate = std::function<T ()>;

/**
 * The complete function format used by AsyncWork.
 *
 * The complete function runs in the main javascript thread after create has completed. The AsyncWorkResult contains
 * the result of executing the create function.
 */
template<typename T>
using AsyncWorkComplete = std::function<void (Napi::Env env, AsyncWorkResult<T>*)>;

/**
 * Manages the internal state of an AsyncWork object.
 *
 * The underlying napi async work cannot be cleaned up until the complete callback has executed. If the async work
 * is destroyed before this happens, the program will crash. The program has to wait for callbacks to quiesce before
 * async work clean up.
 *
 * The lifecycle of async work does not map to a C++ object very well. The solution is to keep a global list of
 * executing async work objects. When the complete callback finishes, the global list entry is removed and the
 * async work can be cleaned up.
 *
 * Note, the napi C++ API has AsyncWorker, but I have had problems with managing the lifecycle on program shutdown and
 * cancelling async operations.
 *
 * @tparam T type of object that the AsyncWork create function returns
 */
template<typename T>
class AsyncWorkState : public std::enable_shared_from_this<AsyncWorkState<T>> {
 public:
    AsyncWorkState(Napi::Env env, AsyncWorkCreate<T>&& create, AsyncWorkComplete<T>&& complete);
    ~AsyncWorkState();

    void Queue();
    void Cancel();
    void Destroy();
    bool IsQueued() const;
    AsyncWorkResult<T>* GetResult() { return &this->result; }

 private:
    using AsyncWorkStateMap = std::unordered_map<const AsyncWorkState<T>*, std::shared_ptr<AsyncWorkState<T>>>;

    static std::shared_ptr<AsyncWorkState<T>> GetState(const AsyncWorkState<T>* state);
    static void EraseState(AsyncWorkState<T>* state);
    static void InsertState(AsyncWorkState<T>* state);

 private:
    static AsyncWorkStateMap sAsyncWorkStateMap;
    static std::mutex sAsyncWorkStateMapMutex;

    Napi::Env env;
    AsyncWorkCreate<T> create;
    AsyncWorkComplete<T> complete;
    napi_async_work work{};
    AsyncWorkResult<T> result{};
    bool isQueued{false};
};

/**
 * Manages the specific use case of creating a native object in a background thread and then processing the
 * native object on the main javascript thread.
 *
 * This class is different from Napi::AsyncWorker in two ways:
 *
 * 1. AsyncWork handles a specific, specialized use case.
 * 2. AsyncWork does a better job of managing the lifecycle of the underlying napi async_work object.
 *
 * @tparam T type of object that the AsyncWork create function returns
 */
template<typename T>
class AsyncWork {
 public:
    AsyncWork() = default;
    ~AsyncWork();

    /**
     * Get the javascript environment.
     *
     * Note: This will return nullptr until Reset() is called.
     */
    Napi::Env Env() const noexcept { return this->env; }

    /**
     * Set the create and complete functions.
     */
    void Reset(Napi::Env env, AsyncWorkCreate<T>&& create, AsyncWorkComplete<T>&& complete);

    /**
     * Submit the create function for background execution.
     *
     * Assumes Reset() has been called.
     */
    void Queue();

    /**
     * Cancel execution.
     *
     * If execution has been queued, the create method may or may not execute (program should handle this
     * accordingly). The complete method will not be called.
     */
    void Cancel();

 private:
    Napi::Env env{nullptr};
    std::shared_ptr<AsyncWorkState<T>> state{};
};

} // namespace ls

#include "AsyncWork-inl.h"
