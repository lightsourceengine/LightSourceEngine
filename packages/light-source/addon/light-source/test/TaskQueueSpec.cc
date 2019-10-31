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
using Napi::TestInfo;
using Napi::TestSuite;

namespace ls {

void WaitForCompleteCalled();
void CompleteCallback();

static bool completeCalled{false};
static std::unique_ptr<Executor> executor;
static std::unique_ptr<TaskQueue> taskQueue;

void TaskQueueSpec(TestSuite* parent) {
    auto spec{ parent->Describe("TaskQueue") };

    spec->beforeEach = [](Napi::Env env) {
        completeCalled = false;
        executor = std::make_unique<Executor>();
        taskQueue = std::make_unique<TaskQueue>();

        taskQueue->Init(executor.get());
    };

    spec->afterEach = [](Napi::Env env) {
        taskQueue.reset();
        executor.reset();
    };

    spec->Describe("Queue()")->tests = {
        {
            "should queue and execute task queue callback",
            [](const TestInfo&) {
                taskQueue->Queue(&CompleteCallback);

                Assert::IsFalse(completeCalled);

                taskQueue->ProcessTasks();

                Assert::IsTrue(completeCalled);
            }
        },
    };

    spec->Describe("Async()")->tests = {
        {
            "should call async function, then queue and execute task queue callback",
            [](const TestInfo&) {
                int32_t resultValue{};
                std::exception_ptr resultException;

                auto task = taskQueue->Async<int32_t>(
                    []() -> int32_t {
                        return 5;
                    },
                    [&](int32_t& value, const std::exception_ptr& e) {
                        completeCalled = true;
                        resultValue = value;
                        resultException = e;
                    });

                Assert::IsFalse(task.WasCancelled());

                WaitForCompleteCalled();

                Assert::Equal(resultValue, 5);
                Assert::IsTrue(!resultException);
            }
        },
        {
            "should handle exception from async function",
            [](const TestInfo&) {
                std::exception_ptr resultException;

                auto task = taskQueue->Async<int32_t>(
                    []() -> int32_t {
                        throw std::runtime_error("error");
                    },
                    [&](int32_t& value, const std::exception_ptr& e) {
                        completeCalled = true;
                        resultException = e;
                    });

                Assert::IsFalse(task.WasCancelled());

                WaitForCompleteCalled();

                Assert::IsTrue(!!resultException);

                try {
                    std::rethrow_exception(resultException);
                } catch (const std::exception& e) {
                    Assert::Equal(std::string(e.what()), std::string("error"));
                }
            }
        },
    };
}

void CompleteCallback() {
    completeCalled = true;
}

void WaitForCompleteCalled() {
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

    Assert::Fail("Timeout waiting for task queue to complete a task.");
}

} // namespace ls
