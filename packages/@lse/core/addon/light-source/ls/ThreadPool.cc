/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/ThreadPool.h>

namespace ls {

ThreadPool::ThreadPool() {
    auto concurrency{ std::thread::hardware_concurrency() };

    if (concurrency <= 0) {
        concurrency = 1u;
    } else {
        concurrency = std::min(concurrency, 4u);
    }

    this->threadPool.reserve(concurrency);

    for (auto i{ 0u }; i < concurrency; i++) {
        auto worker = std::thread([this]() {
            std::function<void()> work;

            while (this->running) {
                this->workQueue.wait_dequeue(work);

                // Shutdown inserts nullptr to indicate worker threads should shutdown.
                if (!work) {
                    break;
                }

                try {
                     work();
                } catch (const std::exception&) {
                    // TODO: ???
                }
            }
        });

        this->threadPool.emplace_back(std::move(worker));
    }
}

ThreadPool::~ThreadPool() {
    this->ShutdownNow();
}

void ThreadPool::Execute(std::function<void()>&& work) {
    assert(!!work);
    assert(this->running);

    this->workQueue.enqueue(std::move(work));
}

void ThreadPool::ShutdownNow() {
    if (!this->running) {
        return;
    }

    // this may trigger some threads to terminate
    this->running = false;

    // when a thread sees a null task, it will return
    std::for_each(this->threadPool.begin(), this->threadPool.end(), [this](std::thread&){
        this->workQueue.enqueue(nullptr);
    });

    // join all the threads
    std::for_each(this->threadPool.begin(), this->threadPool.end(), [](std::thread& t){
        t.join();
    });

    this->threadPool.clear();

    std::function<void()> work;

    // drain the work queue
    while (workQueue.try_dequeue(work)) {
    }
}

} // namespace ls
