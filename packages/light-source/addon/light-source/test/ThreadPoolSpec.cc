/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <ls/ThreadPool.h>
#include <napi-unit.h>

#include <chrono>
#include <memory>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace ls {

const auto TIMEOUT = std::chrono::milliseconds(100);
static std::unique_ptr<ThreadPool> executor;

void ExecutorSpec(TestSuite* parent) {
    auto spec{ parent->Describe("ThreadPool") };

    spec->beforeEach = [](Napi::Env env) {
        executor = std::make_unique<ThreadPool>();
    };

    spec->afterEach = [](Napi::Env env) {
        executor.reset();
    };

    spec->Describe("Execute()")->tests = {
        {
            "should execute a function in the thread pool",
            [](const TestInfo&) {
                std::promise<void> p;

                executor->Execute([&p]() {
                    p.set_value();
                });

                auto result { p.get_future().wait_for(TIMEOUT) };

                Assert::IsTrue(result == std::future_status::ready, "thread timeout");
            }
        },
    };

    spec->Describe("Submit()")->tests = {
        {
            "should call function with int return",
            [](const TestInfo&) {
                auto future = executor->Submit<int32_t>([&]() -> int32_t {
                    return 5;
                });

                Assert::Equal(future.get(), 5);
            }
        },
        {
            "should call function with void return",
            [](const TestInfo&) {
                auto future = executor->Submit<void>([&]() {
                });

                auto result{ future.wait_for(TIMEOUT) };

                Assert::IsTrue(result == std::future_status::ready, "thread timeout");
            }
        },
        {
            "should catch async exceptions",
            [](const TestInfo&) {
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
            [](const TestInfo&) {
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
            [](const TestInfo&) {
                Assert::IsTrue(executor->IsRunning());
            }
        },
    };
}

} // namespace ls
