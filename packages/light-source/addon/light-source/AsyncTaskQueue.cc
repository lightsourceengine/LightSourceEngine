/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "AsyncTaskQueue.h"
#include <fmt/println.h>

namespace ls {

Task::Task(TaskExecuteFunction&& execute, TaskCompleteFunction&& complete) noexcept
    : execute(std::move(execute)), complete(std::move(complete)) {
}

void Task::Cancel() noexcept {
    this->wasCancelled = true;
    this->result.reset();
}

std::shared_ptr<void> Task::GetResult() const noexcept {
    return this->IsSuccess() ? this->result : nullptr;
}

bool Task::IsSuccess() const noexcept {
    return !this->wasCancelled && !this->isError && this->isComplete;
}

bool Task::IsError() const noexcept {
    return !this->wasCancelled && this->isError;
}

const std::string& Task::GetErrorMessage() const noexcept {
    static const std::string empty;

    return this->IsError() ? this->errorMessage : empty;
}

bool Task::WasCancelled() const noexcept {
    return this->wasCancelled;
}

AsyncTaskQueue::AsyncTaskQueue() {
    this->Init();
}

AsyncTaskQueue::~AsyncTaskQueue() noexcept {
    this->Shutdown();
}

void AsyncTaskQueue::Init() {
    auto concurrency{ std::thread::hardware_concurrency() };

    if (concurrency <= 0) {
        concurrency = 1u;
    } else {
        concurrency = std::min(concurrency, 4u);
    }

    this->threadPool.reserve(concurrency);

    for (auto i{ 0u }; i < concurrency; i++) {
        auto worker = std::thread([this]() {
            std::shared_ptr<Task> task;

            while (this->running) {
                this->pendingExecuteQueue.wait_dequeue(task);

                if (!task) {
                    break;
                }

                if (!task->WasCancelled()) {
                    try {
                        task->result = task->execute();
                    } catch (const std::exception& e) {
                        task->errorMessage = e.what();
                        task->isError = true;
                    }

                    this->postExecuteQueue.enqueue(task);
                }
            }
        });

        this->threadPool.emplace_back(std::move(worker));
    }
}

std::shared_ptr<Task> AsyncTaskQueue::Submit(TaskExecuteFunction&& execute, TaskCompleteFunction&& complete) {
    assert(this->running);
    auto task{ std::make_shared<Task>(std::move(execute), std::move(complete)) };

    this->pendingExecuteQueue.enqueue(task);

    return task;
}

bool AsyncTaskQueue::IsRunning() const noexcept {
    return this->running;
}

void AsyncTaskQueue::ProcessTasks() noexcept {
    assert(this->running);
    std::shared_ptr<Task> task;

    while (this->postExecuteQueue.try_dequeue(task)) {
        if (task->WasCancelled()) {
            continue;
        }

        task->isComplete = true;

        try {
            if (task->complete) {
                task->complete(task);
            }
        } catch (const std::exception& e) {
            task->Cancel();
            fmt::println("ResourceManager.ProcessTasks(): Error: {}", e.what());
        }
    }
}

void AsyncTaskQueue::Shutdown() noexcept {
    if (!this->running) {
        return;
    }

    // this may trigger some threads to terminate
    this->running = false;

    // when a thread sees a null task, it will return
    std::for_each(this->threadPool.begin(), this->threadPool.end(), [this](std::thread&){
        this->pendingExecuteQueue.enqueue(std::shared_ptr<Task>());
    });

    // join all the threads
    std::for_each(this->threadPool.begin(), this->threadPool.end(), [](std::thread& t){
        t.join();
    });

    this->threadPool.clear();

    std::shared_ptr<Task> task;

    // drain the execute queue
    while (pendingExecuteQueue.try_dequeue(task)) {
    }

    // drain the complete queue
    while (postExecuteQueue.try_dequeue(task)) {
    }
}

} // namespace ls
