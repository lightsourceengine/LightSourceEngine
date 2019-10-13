/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "Task.h"

namespace ls {

Task::Task(const Task& other) noexcept : state(other.state) {
}

Task::Task(Task&& other) noexcept : state(std::move(other.state)) {
}

void Task::Cancel() noexcept {
    if (this->state) {
        this->state->wasCancelled = true;
    }
}

bool Task::WasCancelled() const noexcept {
    if (this->state) {
        return this->state->wasCancelled;
    } else {
        return false;
    }
}

Task& Task::operator=(Task&& other) noexcept {
    this->state = std::move(other.state);

    return *this;
}

Task& Task::operator=(const Task& other) noexcept {
    this->state = other.state;

    return *this;
}

Task Task::Create() {
    Task task;

    task.state = std::make_shared<Task::State>();

    return task;
}

} // namespace ls
