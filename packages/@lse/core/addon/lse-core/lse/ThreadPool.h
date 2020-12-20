/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <functional>
#include <blockingconcurrentqueue.h>
#include <future>
#include <type_traits>
#include <vector>

namespace ls {

/**
 * Asynchronous function executor backed by a thread pool.
 *
 * ThreadPool is loosely based on Java's ExecutorService classes. The underlying thread pool contains 4 threads. If the
 * number of cpu cores is less than 4, the thread pool size equals the number of cores.
 *
 * This class is thread safe, except for constructor and ShutdownNow(), which should be called from the main thread.
 */
class ThreadPool {
 public:
  ThreadPool();
    ~ThreadPool();

    /**
     * Submits work for execution.
     *
     * If the work function throws an exception, the exception is ignored.
     */
    void Execute(std::function<void()>&& work);

    /**
     * Submits work for execution and returns a future representing that work.
     *
     * The future contains the return value of the work function. If the work function throws an exception, it will
     * be packaged with the future and thrown from future.get().
     */
    template<typename T>
    std::future<T> Submit(std::function<T()>&& work);

    /**
     * Initiates an orderly shutdown in which previously submitted tasks are executed, but no new tasks will be
     * accepted. After shutdown, the executor should not longer be used.
     */
    void ShutdownNow();

    /**
     * Returns true if the executor is running (not shutdown).
     */
    bool IsRunning() noexcept { return this->running; }

 private:
    std::vector<std::thread> threadPool;
    moodycamel::BlockingConcurrentQueue<std::function<void()>> workQueue;
    std::atomic<bool> running{true};
};

namespace internal {

// Hack for lambda capture of objects with no copy constructors. The copy constructor is implemented as a move,
// which is destructive to the object being copied from. This should only be used in specific capture cases and not
// for general use.
template<typename T>
struct MoveOnCopy {
    MoveOnCopy() noexcept = default;
    MoveOnCopy(MoveOnCopy<T>&& other) noexcept = default;
    MoveOnCopy(T&& value) noexcept : value(std::move(value)) {} // NOLINT
    MoveOnCopy(const MoveOnCopy<T>& other) noexcept : value(std::move(other.value)) {}

    mutable T value{};
};

} // namespace internal

template<typename T>
std::future<T> ThreadPool::Submit(std::function<T()>&& work) {
    assert(!!work);
    assert(this->running);

    auto packagedTask{ std::packaged_task<T()>(std::move(work)) };
    auto future{ packagedTask.get_future() };
    using MoveOnCopy = internal::MoveOnCopy<std::packaged_task<T()>>;

    this->workQueue.enqueue([p = MoveOnCopy(std::move(packagedTask))]() mutable {
        p.value();
    });

    return future;
}

} // namespace ls
