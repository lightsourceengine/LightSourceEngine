/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <Executor.h>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>
#include <condition_variable>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

const auto TIMEOUT = std::chrono::milliseconds(100);
static std::unique_ptr<Executor> executor;

void ExecutorSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("Executor") };
    auto assert{ Assert(env) };

    spec->beforeEach = [](const Napi::CallbackInfo& info) {
        executor = std::make_unique<Executor>();
    };

    spec->afterEach = [](const Napi::CallbackInfo& info) {
        executor.reset();
    };

    spec->Describe("Execute()")->tests = {
        {
            "should execute a function in the thread pool",
            [assert](const Napi::CallbackInfo& info) {
                std::mutex mutex;
                std::condition_variable cv;

                executor->Execute([&]() {
                    cv.notify_one();
                });

                std::unique_lock<std::mutex> lock(mutex);
                auto result{ cv.wait_for(lock, TIMEOUT) };

                assert.IsTrue(result != std::cv_status::timeout);
            }
        },
    };

    spec->Describe("Submit()")->tests = {
        {
            "should call function with int return",
            [assert](const Napi::CallbackInfo& info) {
                auto future = executor->Submit<int32_t>([&]() -> int32_t {
                    return 5;
                });

                assert.Equal(future.get(), 5);
            }
        },
        {
            "should call function with void return",
            [assert](const Napi::CallbackInfo& info) {
                auto future = executor->Submit<void>([&]() {
                });

                auto result{ future.wait_for(TIMEOUT) };

                assert.IsTrue(result == std::future_status::ready);
            }
        },
        {
            "should catch async exceptions",
            [assert](const Napi::CallbackInfo& info) {
                auto future = executor->Submit<void>([&]() {
                    throw std::runtime_error("error");
                });

                try {
                    future.get();
                } catch (const std::exception& e) {
                    assert.Equal(std::string(e.what()), std::string("error"));
                    return;
                }

                assert.Fail("Expected exception from future.get()");
            }
        },
    };

    spec->Describe("ShutdownNow()")->tests = {
        {
            "should be idempotent",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(executor->IsRunning());
                executor->ShutdownNow();
                assert.IsFalse(executor->IsRunning());
                executor->ShutdownNow();
                assert.IsFalse(executor->IsRunning());
            }
        },
    };

    spec->Describe("IsRunning()")->tests = {
        {
            "should be running",
            [assert](const Napi::CallbackInfo& info) {
                assert.IsTrue(executor->IsRunning());
            }
        },
    };
}

} // namespace ls
