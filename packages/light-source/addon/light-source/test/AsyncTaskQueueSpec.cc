/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "LightSourceSpec.h"
#include <AsyncTaskQueue.h>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <fmt/format.h>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

struct TestTaskResult : public TaskResult {
    explicit TestTaskResult(int value) : value(value) {
    }

    int value{};
};

void WaitForTaskDone(std::shared_ptr<AsyncTaskQueue> queue, std::shared_ptr<Task> task);
void CompleteCallback(std::shared_ptr<TaskResult> result);
void ErrorCallback(const std::string& message);

static std::shared_ptr<AsyncTaskQueue> asyncTaskQueue;
static bool completeCalled{false};
static bool errorCalled{false};
static std::string errorMessage; // NOLINT(runtime/string)

void AsyncTaskQueueSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("AsyncTaskQueue") };
    auto assert{ Assert(env) };

    spec->beforeEach = [](const Napi::CallbackInfo& info) mutable {
        asyncTaskQueue.reset(new AsyncTaskQueue());
        asyncTaskQueue->Init();
        completeCalled = false;
        errorCalled = false;
        errorMessage.clear();
    };

    spec->afterEach = [](const Napi::CallbackInfo& info) mutable {
        if (asyncTaskQueue) {
            asyncTaskQueue.reset();
        }
    };

    spec->Describe("Submit()")->tests = {
        {
            "should execute submitted task",
            [assert](const Napi::CallbackInfo& info) mutable {
                auto task = std::make_shared<Task>(
                    []() -> std::shared_ptr<TaskResult> {
                        return std::make_shared<TestTaskResult>(5);
                    },
                    CompleteCallback,
                    ErrorCallback);

                asyncTaskQueue->Submit(task);

                WaitForTaskDone(asyncTaskQueue, task);

                assert.IsTrue(task->IsDone());
                assert.IsTrue(task->GetResult() != nullptr);
                assert.IsTrue(completeCalled);
                assert.IsTrue(task->HasResult());
                assert.IsFalse(errorCalled);
                assert.IsFalse(task->HasError());
                assert.Equal(std::static_pointer_cast<TestTaskResult>(task->GetResult())->value, 5);
            }
        },
        {
            "should handle exception in execute method as an error",
            [assert](const Napi::CallbackInfo& info) mutable {
                static std::string errorText{"test error message"};

                auto task = std::make_shared<Task>(
                    []() -> std::shared_ptr<TaskResult> {
                        throw std::runtime_error(errorText);
                    },
                    CompleteCallback,
                    ErrorCallback);

                asyncTaskQueue->Submit(task);

                WaitForTaskDone(asyncTaskQueue, task);

                assert.IsTrue(task->IsDone());
                assert.IsFalse(completeCalled);
                assert.IsFalse(task->HasResult());
                assert.IsTrue(errorCalled);
                assert.IsTrue(task->HasError());
                assert.Equal(errorMessage, errorText);
            }
        }
    };

    spec->Describe("Cancel()")->tests = {
        {
            "should cancel task",
            [assert](const Napi::CallbackInfo& info) mutable {
                auto task = std::make_shared<Task>(
                    []() -> std::shared_ptr<TaskResult> {
                        return {};
                    },
                    CompleteCallback,
                    ErrorCallback);

                asyncTaskQueue->Submit(task);
                task->Cancel();

                WaitForTaskDone(asyncTaskQueue, task);

                assert.IsTrue(task->IsDone());
                assert.IsFalse(task->HasResult());
                assert.IsFalse(errorCalled);
                assert.IsFalse(task->HasError());
            }
        },
    };

    spec->Describe("Shutdown()")->tests = {
        {
            "should be idempotent",
            [](const Napi::CallbackInfo& info) mutable {
                asyncTaskQueue->Shutdown();
                asyncTaskQueue->Shutdown();
            }
        },
    };
}

void CompleteCallback(std::shared_ptr<TaskResult> result) {
    completeCalled = true;
}

void ErrorCallback(const std::string& message) {
    errorCalled = true;
    errorMessage = message;
}

void WaitForTaskDone(std::shared_ptr<AsyncTaskQueue> queue, std::shared_ptr<Task> task) {
    auto loops{100};

    while (loops-- && !task->IsDone()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        queue->ProcessCompleteTasks();
    }
}

} // namespace ls
