/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <thread>
#include <functional>
#include <concurrentqueue.h>
#include "Task.h"
#include "Executor.h"

namespace ls {

/**
 * Task execution queue.
 *
 * The task queue is the native layer's equivalent of setImmediate (or queueMicrotask). Native code can submit
 * a function to the queue and the function is executed later (next frame) on Node's main thread.
 *
 * The next use case is executing an asynchronous function on a background thread. When the async work is complete,
 * a second function needs to be run on Node's main thread. This can be done with Async().
 */
class TaskQueue {
 public:
    ~TaskQueue();

    /**
     * Add a task to the queue.
     *
     * The callback will be run when ProcessTasks() is called (FIFO ordering).
     *
     * The return object allows for cancelling the task. If the task has not executed, cancelling guarantees the
     * task will not be called.
     */
    Task Queue(std::function<void()>&& callback);

    /**
     * Add a task, conditional on asynchronous work.
     *
     * The async function is run in a thread pool (not the main thread). When the async work finishes (success or
     * exception), the callback with async return value or exception is added to the task queue.
     *
     * The return object allows for cancelling the task. Cancelling is guaranteed to cancel the callback, but may or
     * may not cancel the async work (if it's in progress).
     */
    template<typename T>
    Task Async(std::function<T()>&& async, std::function<void(T&&, const std::exception_ptr&)>&& callback);

    /**
     * Run all queued tasks.
     *
     * This method should be called from the main thread (once per frame).
     */
    void ProcessTasks();


    /**
     * Initialize the queue with a backing thread pool (used for Async() method).
     */
    void Init(Executor* executor) noexcept;

    /**
     * Initiates an orderly shutdown in which previously submitted tasks are dropped. No new tasks will be
     * accepted. After shutdown, the queue should not longer be used.
     */
    void ShutdownNow();

 private:
    Executor* executor{};
    moodycamel::ConcurrentQueue<std::function<void()>> queue;
};

template<typename T>
Task TaskQueue::Async(std::function<T()>&& async, std::function<void(T&&, const std::exception_ptr&)>&& callback) {
    auto task{ Task::Create() };

    this->executor->Execute([this, task, async = std::move(async), callback = std::move(callback)]() mutable {
        if (task.WasCancelled()) {
            return;
        }

        internal::Movable<T> result;
        std::exception_ptr exceptionPtr;

        try {
            result.value = async();
        } catch (const std::exception& e) {
            exceptionPtr = std::current_exception();
        }

        if (task.WasCancelled()) {
            return;
        }

        this->queue.enqueue([task, result = std::move(result), exceptionPtr, callback = std::move(callback)]() mutable {
            if (task.WasCancelled()) {
                return;
            }

            callback(std::move(result.value), exceptionPtr);
        });
    });

    return task;
}

} // namespace ls
