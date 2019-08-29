/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <string>
#include <vector>
#include <functional>
#include <fmt/format.h>

namespace ls {

typedef std::function<void(const Napi::CallbackInfo&)> TestFunction;

/**
 * Test case function and description.
 *
 * Maps to parameters of Mocha's test declaration: it(description, function).
 */
struct Test {
    std::string description;
    TestFunction func;
};

/**
 * Interface for data owned by a TestGroup and shared between test cases.
 */
class TestContext {
    virtual ~TestContext() = default;
};

/**
 * Container for test cases and child test groups.
 *
 * Maps to Mocha's describe declaration. TestGroup specifies the describe description, test cases (it) and
 * lifecycle hooks (before, after, beforeEach, afterEach).
 */
class TestGroup : public Napi::ObjectWrap<TestGroup> {
 public:
    explicit TestGroup(const Napi::CallbackInfo& info);
    virtual ~TestGroup();

    static Napi::Object New(Napi::Env env, const std::string& description);

    /**
     * Add a child group.
     */
    TestGroup* AddChild(const std::string& description);
    TestGroup* Describe(const std::string& description); // alias for AddChild()

    /**
     * Get TestContext as a test specific subclass.
     */
    template<class T>
    std::shared_ptr<T> GetContextAs() {
        return std::static_pointer_cast<T>(this->context).get();
    }

 public:
    // Note: These are exposed publicly for test writing convenience.

    // List of test cases. Optional.
    std::vector<Test> tests;
    // Function executed before tests are run. Optional.
    TestFunction before;
    // Function executed after all tests are run. Optional.
    TestFunction after;
    // Function executed before each test case is run. Optional.
    TestFunction beforeEach;
    // Function executed after each test case is run. Optional.
    TestFunction afterEach;
    // Test specific data attached this group. Optional.
    std::shared_ptr<TestContext> context;

 private:
    Napi::Value GetDescription(const Napi::CallbackInfo& info);
    Napi::Value GetTests(const Napi::CallbackInfo& info);
    Napi::Value GetChildren(const Napi::CallbackInfo& info);
    Napi::Value GetBefore(const Napi::CallbackInfo& info);
    Napi::Value GetAfter(const Napi::CallbackInfo& info);
    Napi::Value GetBeforeEach(const Napi::CallbackInfo& info);
    Napi::Value GetAfterEach(const Napi::CallbackInfo& info);

 private:
    std::string description;
    std::vector<TestGroup*> children;
};

/**
 * Test assertion methods.
 *
 * To be compatible with the javascript test environment, assertion errors are thrown using napi exceptions.
 */
class Assert {
 public:
    explicit Assert(Napi::Env env) : env(env) {
    }

    /**
     * Checks that two values are equal using the == operator.
     */
    template<typename T>
    void Equal(const T& value, const T& expected) const {
        if (!(value == expected)) {
            throw Napi::Error::New(this->env, fmt::format("Expected {} to be equal to {}", value, expected));
        }
    }

    /**
     * Checks that the value is true.
     */
    void IsTrue(bool value) const {
        if (!value) {
            throw Napi::Error::New(this->env, "Expected true");
        }
    }

    /**
     * Checks that the value is false.
     */
    void IsFalse(bool value) const {
        if (value) {
            throw Napi::Error::New(this->env, "Expected false");
        }
    }

 private:
    Napi::Env env;
};

} // namespace ls
