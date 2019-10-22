/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <Executor.h>
#include <memory>
#include <chrono>
#include <mutex>

using Napi::Assert;
using Napi::TestSuite;

namespace ls {

const auto TIMEOUT = std::chrono::milliseconds(100);
static std::unique_ptr<Executor> executor;

void ExecutorSpec(Napi::Env env, TestSuite* parent) {
    auto spec{ parent->Describe("Executor") };

    spec->beforeEach = [](const Napi::CallbackInfo& info) {
        executor = std::make_unique<Executor>();
    };

    spec->afterEach = [](const Napi::CallbackInfo& info) {
        executor.reset();
    };

    spec->Describe("Execute()")->tests = {
        {
            "should execute a function in the thread pool",
            [](const Napi::CallbackInfo& info) {
                std::mutex mutex;
                std::condition_variable cv;

                executor->Execute([&]() {
                    cv.notify_one();
                });

                std::unique_lock<std::mutex> lock(mutex);
                auto result{ cv.wait_for(lock, TIMEOUT) };

                Assert::IsTrue(result != std::cv_status::timeout);
            }
        },
    };

    spec->Describe("Submit()")->tests = {
        {
            "should call function with int return",
            [](const Napi::CallbackInfo& info) {
                auto future = executor->Submit<int32_t>([&]() -> int32_t {
                    return 5;
                });

                Assert::Equal(future.get(), 5);
            }
        },
        {
            "should call function with void return",
            [](const Napi::CallbackInfo& info) {
                auto future = executor->Submit<void>([&]() {
                });

                auto result{ future.wait_for(TIMEOUT) };

                Assert::IsTrue(result == std::future_status::ready);
            }
        },
        {
            "should catch async exceptions",
            [](const Napi::CallbackInfo& info) {
                auto future = executor->Submit<void>([&]() {
                    throw std::runtime_error("error");
                });

                try {
                    future.get();
                } catch (const std::exception& e) {
                    Assert::Equal(std::string(e.what()), std::string("error"));
                    return;
                }

                Assert::Fail("Expected exception from future.get()");
            }
        },
    };

    spec->Describe("ShutdownNow()")->tests = {
        {
            "should be idempotent",
            [](const Napi::CallbackInfo& info) {
                Assert::IsTrue(executor->IsRunning());
                executor->ShutdownNow();
                Assert::IsFalse(executor->IsRunning());
                executor->ShutdownNow();
                Assert::IsFalse(executor->IsRunning());
            }
        },
    };

    spec->Describe("IsRunning()")->tests = {
        {
            "should be running",
            [](const Napi::CallbackInfo& info) {
                Assert::IsTrue(executor->IsRunning());
            }
        },
    };
}

} // namespace ls
