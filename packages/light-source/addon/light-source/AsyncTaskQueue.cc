/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "AsyncTaskQueue.h"
#include <fmt/format.h>

namespace ls {

void AsyncTaskQueue::Init() noexcept {
    assert(threadPool.empty());

    auto concurrency{ std::thread::hardware_concurrency() };

    if (concurrency == 0) {
        concurrency = 1u;
    } else {
        concurrency = std::min(concurrency, 4u);
    }

    for (auto i{ 0u }; i < concurrency; i++) {
        this->threadPool.push_back(std::thread([this]() {
            std::shared_ptr<Task> task;

            while (this->running) {
                executeQueue.wait_dequeue(task);

                if (!task) {
                    break;
                }

                if (task->cancelled) {
                    continue;
                }

                try {
                    task->result = task->execute();
                } catch (std::exception& e) {
                    task->asyncHasError = true;
                    task->errorMessage = e.what();
                }

                if (!task->cancelled) {
                    completeQueue.enqueue(task);
                }
            }
        }));
    }
}

void AsyncTaskQueue::Submit(std::shared_ptr<Task> task) {
    assert(task);
    assert(this->running);

    this->executeQueue.enqueue(task);
}

void AsyncTaskQueue::ProcessCompleteTasks() noexcept {
    assert(this->running);
    std::shared_ptr<Task> task;

    while (this->completeQueue.try_dequeue(task)) {
        task->isDone = true;

        if (task->cancelled) {
            continue;
        }

        try {
            if (task->asyncHasError && task->error) {
                task->hasError = true;
                task->error(task->errorMessage);
            } else {
                task->hasResult = true;

                if (task->complete) {
                    task->complete(task->result);
                }
            }
        } catch (std::exception& e) {
            task->Cancel();
            fmt::println("ResourceManager.ProcessCompleteTasks(): Error: {}", e.what());
        }
    }
}

void AsyncTaskQueue::Shutdown() noexcept {
    // this may trigger some threads to terminate
    this->running = false;

    // when a thread sees a null task, it will return
    std::for_each(this->threadPool.begin(), this->threadPool.end(), [this](std::thread&){
        this->executeQueue.enqueue(std::shared_ptr<Task>());
    });

    // join all the threads
    std::for_each(this->threadPool.begin(), this->threadPool.end(), [](std::thread& t){ t.join(); });

    this->threadPool.clear();

    std::shared_ptr<Task> task;

    // drain the execute queue
    while (executeQueue.try_dequeue(task)) {
    }

    // drain the complete queue
    while (completeQueue.try_dequeue(task)) {
    }
}

} // namespace ls
