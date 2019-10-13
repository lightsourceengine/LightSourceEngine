/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <TaskQueue.h>
#include <memory>
#include <thread>
#include <chrono>
#include <string>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

void WaitForCompleteCalled(const Assert& assert);
void CompleteCallback();

static bool completeCalled{false};
static std::unique_ptr<Executor> executor;
static std::unique_ptr<TaskQueue> taskQueue;

void TaskQueueSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("TaskQueue") };
    auto assert{ Assert(env) };

    spec->beforeEach = [](const Napi::CallbackInfo& info) {
        completeCalled = false;
        executor = std::make_unique<Executor>();
        taskQueue = std::make_unique<TaskQueue>();

        taskQueue->Init(executor.get());
    };

    spec->afterEach = [](const Napi::CallbackInfo& info) {
        taskQueue.reset();
        executor.reset();
    };

    spec->Describe("Queue()")->tests = {
        {
            "should queue and execute task queue callback",
            [assert](const Napi::CallbackInfo& info) {
                taskQueue->Queue(CompleteCallback);

                assert.IsFalse(completeCalled);

                taskQueue->ProcessTasks();

                assert.IsTrue(completeCalled);
            }
        },
    };

    spec->Describe("Async()")->tests = {
        {
            "should call async function, then queue and execute task queue callback",
            [assert](const Napi::CallbackInfo& info) {
                int32_t resultValue{};
                std::exception_ptr resultException;

                auto task = taskQueue->Async<int32_t>(
                    []() -> int32_t {
                        return 5;
                    },
                    [&](int32_t&& value, const std::exception_ptr& e) {
                        completeCalled = true;
                        resultValue = value;
                        resultException = e;
                    });

                assert.IsFalse(task.WasCancelled());

                WaitForCompleteCalled(assert);

                assert.Equal(resultValue, 5);
                assert.IsTrue(!resultException);
            }
        },
        {
            "should handle exception from async function",
            [assert](const Napi::CallbackInfo& info) {
                int32_t resultValue{};
                std::exception_ptr resultException;

                auto task = taskQueue->Async<int32_t>(
                    []() -> int32_t {
                        throw std::runtime_error("error");
                    },
                    [&](int32_t&& value, const std::exception_ptr& e) {
                        completeCalled = true;
                        resultValue = value;
                        resultException = e;
                    });

                assert.IsFalse(task.WasCancelled());

                WaitForCompleteCalled(assert);

                assert.IsTrue(!!resultException);

                try {
                    std::rethrow_exception(resultException);
                } catch (const std::exception& e) {
                    assert.Equal(std::string(e.what()), std::string("error"));
                    return;
                }

                assert.Fail("Expected exception with 'error' message.");
            }
        },
    };
}

void CompleteCallback() {
    completeCalled = true;
}

void WaitForCompleteCalled(const Assert& assert) {
    auto loops{100};

    while (loops--) {
        taskQueue->ProcessTasks();

        if (completeCalled) {
            return;
        }

        if (loops >= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    assert.Fail("Timeout waiting for task queue to complete a task.");
}

} // namespace ls
