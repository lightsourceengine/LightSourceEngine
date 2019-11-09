/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <atomic>
#include <memory>

namespace ls {

class TaskQueue;

/**
 * State of a task submitted to a TaskQueue.
 */
class Task {
 public:
    Task() noexcept = default;
    Task(const Task& other) noexcept;
    Task(Task&& other) noexcept;

    /**
     * Cancel a submitted task.
     */
    void Cancel() noexcept;

    /**
     * Returns true if the task was cancelled.
     */
    bool WasCancelled() const noexcept;

    Task& operator=(Task&& other) noexcept;
    Task& operator=(const Task& other) noexcept;

 private:
    struct State {
        std::atomic<bool> wasCancelled{false};
    };

    static Task Create();

 private:
    std::shared_ptr<State> state;

    friend class TaskQueue;
};

} // namespace ls
