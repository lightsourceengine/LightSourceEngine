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

class AsyncTaskQueue;

struct TaskResult {
};

class Task {
 public:
    typedef std::function<std::shared_ptr<TaskResult> ()> ExecuteFunction;
    typedef std::function<void(std::shared_ptr<TaskResult>)> CompleteFunction;
    typedef std::function<void(const std::string&)> ErrorFunction;

    explicit Task(ExecuteFunction&& execute, CompleteFunction&& complete = nullptr,
        ErrorFunction&& error = nullptr) : execute(execute), complete(complete), error(error) {
    }

    void Cancel() noexcept {
        this->cancelled = true;
    }

    std::shared_ptr<TaskResult> GetResult() const noexcept {
        return this->resultAvailable ? this->result : nullptr;
    }

 private:
    ExecuteFunction execute;
    CompleteFunction complete;
    ErrorFunction error;
    std::atomic<bool> cancelled{false};
    bool resultAvailable{false};
    std::shared_ptr<TaskResult> result;
    bool hasError{false};
    std::string errorMessage;

    friend AsyncTaskQueue;
};

class AsyncTaskQueue {
 public:
    ~AsyncTaskQueue() {
        this->Shutdown();
    }

    void Init() noexcept;
    void Shutdown() noexcept;

    void Submit(std::shared_ptr<Task> task);
    void ProcessCompleteTasks() noexcept;

 private:
    std::vector<std::thread> threadPool;
    moodycamel::BlockingConcurrentQueue<std::shared_ptr<Task>> executeQueue;
    moodycamel::ConcurrentQueue<std::shared_ptr<Task>> completeQueue;
    std::atomic<bool> running{true};
};

} // namespace ls
