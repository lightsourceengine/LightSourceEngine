/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include <lse/ThreadPool.h>
#include <napi-unit.h>

#include <chrono>
#include <memory>

using Napi::Assert;
using Napi::TestInfo;
using Napi::TestSuite;

namespace lse {

const auto TIMEOUT = std::chrono::milliseconds(100);
static std::unique_ptr<ThreadPool> sThreadPool;

void ThreadPoolSpec(TestSuite* parent) {
  auto spec{ parent->Describe("ThreadPool") };

  spec->beforeEach = [](Napi::Env env) {
    sThreadPool = std::make_unique<ThreadPool>();
  };

  spec->afterEach = [](Napi::Env env) {
    sThreadPool.reset();
  };

  spec->Describe("Execute()")->tests = {
      {
          "should execute a function in the thread pool",
          [](const TestInfo&) {
            std::promise<void> p;

            sThreadPool->Execute([&p]() {
              p.set_value();
            });

            auto result{ p.get_future().wait_for(TIMEOUT) };

            Assert::IsTrue(result == std::future_status::ready, "thread timeout");
          }
      },
  };

  spec->Describe("Submit()")->tests = {
      {
          "should call function with int return",
          [](const TestInfo&) {
            auto future = sThreadPool->Submit<int32_t>([&]() -> int32_t {
              return 5;
            });

            Assert::Equal(future.get(), 5);
          }
      },
      {
          "should call function with void return",
          [](const TestInfo&) {
            auto future = sThreadPool->Submit<void>([&]() {
            });

            auto result{ future.wait_for(TIMEOUT) };

            Assert::IsTrue(result == std::future_status::ready, "thread timeout");
          }
      },
      {
          "should catch async exceptions",
          [](const TestInfo&) {
            auto future = sThreadPool->Submit<void>([&]() {
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
            Assert::IsTrue(sThreadPool->IsRunning());
            sThreadPool->ShutdownNow();
            Assert::IsFalse(sThreadPool->IsRunning());
            sThreadPool->ShutdownNow();
            Assert::IsFalse(sThreadPool->IsRunning());
          }
      },
  };

  spec->Describe("IsRunning()")->tests = {
      {
          "should be running",
          [](const TestInfo&) {
            Assert::IsTrue(sThreadPool->IsRunning());
          }
      },
  };
}

} // namespace lse
