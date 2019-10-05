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

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

void WaitForTaskDone(const Assert& assert, const std::shared_ptr<AsyncTaskQueue>& queue,
        const std::shared_ptr<Task>& task);
void CompleteCallback(std::shared_ptr<Task> task);

static std::shared_ptr<AsyncTaskQueue> asyncTaskQueue;
static bool completeCalled{false};
static std::string errorMessage; // NOLINT(runtime/string)

void AsyncTaskQueueSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("AsyncTaskQueue") };
    auto assert{ Assert(env) };

    spec->beforeEach = [](const Napi::CallbackInfo& info) mutable {
        asyncTaskQueue.reset(new AsyncTaskQueue());
        completeCalled = false;
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
                auto task = asyncTaskQueue->Submit(
                    []() -> std::shared_ptr<void> {
                        return std::make_shared<int>(5);
                    },
                    CompleteCallback);

                assert.IsNotNull(task);

                WaitForTaskDone(assert, asyncTaskQueue, task);

                assert.IsNotNull(task->GetResult());
                assert.IsTrue(completeCalled);
                assert.IsTrue(task->IsSuccess());
                assert.IsFalse(task->IsError());
                assert.IsFalse(task->WasCancelled());
                assert.Equal(*task->GetResultAs<int>(), 5);
            }
        },
        {
            "should handle exception in execute method as an error",
            [assert](const Napi::CallbackInfo& info) mutable {
                static const std::string errorText{"test error message"};

                auto task = asyncTaskQueue->Submit(
                    []() -> std::shared_ptr<void> {
                        throw std::runtime_error(errorText);
                    },
                    CompleteCallback);

                assert.IsNotNull(task);

                WaitForTaskDone(assert, asyncTaskQueue, task);

                assert.IsTrue(completeCalled);
                assert.IsFalse(task->IsSuccess());
                assert.IsTrue(task->IsError());
                assert.IsFalse(task->WasCancelled());
                assert.Equal(task->GetErrorMessage(), errorText);
            }
        }
    };

    spec->Describe("Cancel()")->tests = {
        {
            "should cancel task",
            [assert](const Napi::CallbackInfo& info) mutable {
                auto task = asyncTaskQueue->Submit(
                    []() -> std::shared_ptr<void> {
                        return {};
                    },
                    CompleteCallback);

                task->Cancel();

                WaitForTaskDone(assert, asyncTaskQueue, task);

                assert.IsFalse(task->IsSuccess());
                assert.IsFalse(task->IsError());
                assert.IsTrue(task->WasCancelled());
            }
        },
    };

    spec->Describe("Shutdown()")->tests = {
        {
            "should be idempotent",
            [assert](const Napi::CallbackInfo& info) mutable {
                assert.IsTrue(asyncTaskQueue->IsRunning());
                asyncTaskQueue->Shutdown();
                assert.IsFalse(asyncTaskQueue->IsRunning());
                asyncTaskQueue->Shutdown();
                assert.IsFalse(asyncTaskQueue->IsRunning());
            }
        },
    };
}

void CompleteCallback(std::shared_ptr<Task> task) {
    completeCalled = true;
}

void WaitForTaskDone(const Assert& assert, const std::shared_ptr<AsyncTaskQueue>& queue,
        const std::shared_ptr<Task>& task) {
    auto loops{100};

    while (loops--) {
        queue->ProcessTasks();

        if (task->IsError() || task->IsSuccess() || task->WasCancelled()) {
            return;
        }

        if (loops >= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    assert.Fail("Timeout waiting for async queue to complete task.");
}

} // namespace ls
