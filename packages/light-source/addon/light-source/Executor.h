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
 * Executor is loosely based on Java's ExecutorService classes. The underlying thread pool contains 4 threads. If the
 * number of cpu cores is less than 4, the thread pool size equals the number of cores.
 *
 * This class is thread safe, except for constructor and ShutdownNow(), which should be called from the main thread.
 */
class Executor {
 public:
    Executor();
    ~Executor();

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
template<typename T>
struct Movable {
    Movable() = default;
    Movable(T&& value) : value(std::move(value)) {} // NOLINT
    Movable(Movable<T>&& other) : value(std::move(other.value)) {}
    Movable(const Movable<T>& other) : value(std::move(other.value)) {}

    mutable T value;
};
} // namespace internal

template<typename T>
std::future<T> Executor::Submit(std::function<T()>&& work) {
    assert(!!work);
    assert(this->running);

    std::packaged_task<T()> task(std::move(work));
    internal::Movable<std::packaged_task<T()>> p(std::move(task));
    auto future{ p.value.get_future() };

    this->workQueue.enqueue([p = std::move(p)]() {
        p.value();
    });

    return future;
}

} // namespace ls
