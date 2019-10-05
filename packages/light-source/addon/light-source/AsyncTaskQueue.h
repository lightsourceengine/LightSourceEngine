/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <string>
#include <concurrentqueue.h>
#include <blockingconcurrentqueue.h>

namespace ls {

class Task;

typedef std::function<std::shared_ptr<void> ()> TaskExecuteFunction;
typedef std::function<void(std::shared_ptr<Task>)> TaskCompleteFunction;

/**
 * Manages asynchronous task execution in a javascript environment.
 *
 * The task queue manages executing a function in a thread pool and processing the result on the main javascript
 * thread.
 *
 * Technically, the Node runtime can handle manage async work through it's event loop (and Napi exposes support with
 * AsyncWorker). In practice, acquiring the lock to submit work to Node's event loop can be very expensive. On some
 * platforms, like rpi and Node 9/10, sometimes acquiring the lock takes longer than doing the async work being queued.
 * In addition, there is a need for controlling cancelling tasks, managing callbacks after async execution and clean
 * shutdown of async work.
 */
class AsyncTaskQueue {
 public:
    AsyncTaskQueue();
    ~AsyncTaskQueue() noexcept;

    /**
     * Queue a task for execution.
     *
     * The execute function will be called in a thread pool thread (and should not access the javascript context). The
     * complete function will be called after execute has completed and ProcessTask() has been called.
     *
     ( The returned Task instance contains the state of the execution.
     */
    std::shared_ptr<Task> Submit(TaskExecuteFunction&& execute, TaskCompleteFunction&& complete = nullptr);

    /**
     * Process complete callbacks of any tasks that have finished asynchronous execution.
     *
     * This is intended to be called on the main javascript thread.
     */
    void ProcessTasks() noexcept;

    /**
     * Shutdown the queue.
     *
     * The thread pool is immediately shutdown. If any tasks are in the thread pool, some may need to finish executing.
     * However, complete callbacks will not occur.
     *
     * After calling this method, the queue will no longer accept submissions.
     */
    void Shutdown() noexcept;

    /**
     * Is the queue currently accepting execution submissions?
     */
    bool IsRunning() const noexcept;

 private:
    void Init();

 private:
    // Each thread in the pool is waiting on pendingExecuteQueue.
    std::vector<std::thread> threadPool;
    // Tasks waiting for execution on the thread pool.
    moodycamel::BlockingConcurrentQueue<std::shared_ptr<Task>> pendingExecuteQueue;
    // Tasks that have executing, but waiting for ProcessTasks() to be called on the main javascript thread.
    moodycamel::ConcurrentQueue<std::shared_ptr<Task>> postExecuteQueue;
    std::atomic<bool> running{true};
};

/**
 * State of work executed by an AsyncTaskQueue.
 */
class Task {
 public:
    explicit Task(TaskExecuteFunction&& execute, TaskCompleteFunction&& complete = nullptr) noexcept;

    /**
     * Cancel the task.
     *
     * Upon Cancel(), the task will immediately move to the cancelled state. The complete function will not be called,
     * but there is no guarantee that the execute function will be cancelled, as it may be in progress.
     */
    void Cancel() noexcept;

    /**
     * Get the result of the execute function.
     *
     * The task must be in the success state or nullptr will be returned.
     */
    std::shared_ptr<void> GetResult() const noexcept;

    /**
     * Cast result of execute function.
     */
    template<typename T>
    std::shared_ptr<T> GetResultAs() const noexcept {
        return std::static_pointer_cast<T>(this->GetResult());
    }

    /**
     * Checks whether the task is in the success state.
     *
     * Task is in the success state if the execute function ran successfully.
     */
    bool IsSuccess() const noexcept;

    /**
     * Checks whether the task is in the error state.
     *
     * Task is in the error state when an exception is thrown by the execute function and the task was not cancelled.
     */
    bool IsError() const noexcept;

    /**
     * Get the task error message.
     *
     * The error message is available when the task is in the error state. Otherwise, the empty string is returned.
     */
    const std::string& GetErrorMessage() const noexcept;

    /**
     * Checks whether the task has been cancelled.
     */
    bool WasCancelled() const noexcept;

 private:
    TaskExecuteFunction execute;
    TaskCompleteFunction complete;
    std::atomic<bool> wasCancelled{false};
    std::atomic<bool> isError{false};
    bool isComplete{false};
    std::shared_ptr<void> result;
    std::string errorMessage;

    friend AsyncTaskQueue;
};

} // namespace ls
