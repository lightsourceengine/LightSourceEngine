/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TaskQueue.h"
#include <ls/Log.h>

namespace ls {

TaskQueue::~TaskQueue() {
    this->ShutdownNow();
}

Task TaskQueue::Queue(std::function<void()>&& callback) {
    auto task{ Task::Create() };

    this->queue.enqueue({ std::move(callback), task });

    return task;
}

void TaskQueue::ProcessTasks() {
    DeferredCallback callback;

    while (this->queue.try_dequeue(callback)) {
        if (callback.state.WasCancelled()) {
            continue;
        }

        try {
            callback.func();
        } catch (const std::exception& e) {
            LOG_ERROR(e);
        }
    }
}

void TaskQueue::ShutdownNow() {
    DeferredCallback callback;

    while (this->queue.try_dequeue(callback)) {
    }

    this->executor = nullptr;
}

void TaskQueue::Init(Executor* executor) noexcept {
    this->executor = executor;
}

} // namespace ls
