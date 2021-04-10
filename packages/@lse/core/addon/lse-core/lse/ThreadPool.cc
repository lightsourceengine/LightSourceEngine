/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include <lse/ThreadPool.h>

namespace lse {

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
  std::for_each(this->threadPool.begin(), this->threadPool.end(), [this](std::thread&) {
    this->workQueue.enqueue(nullptr);
  });

  // join all the threads
  std::for_each(this->threadPool.begin(), this->threadPool.end(), [](std::thread& t) {
    t.join();
  });

  this->threadPool.clear();

  std::function<void()> work;

  // drain the work queue
  while (workQueue.try_dequeue(work)) {
  }
}

} // namespace lse
