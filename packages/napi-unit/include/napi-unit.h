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
#include <initializer_list>
#include <sstream>

namespace Napi {

class TestSuite;

typedef std::function<void(const Napi::CallbackInfo&)> TestFunction;
typedef std::function<void(Napi::Env, TestSuite*)> TestBuilderFunction;

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
 * Interface for data owned by a TestSuite and shared between test cases.
 */
class TestContext {
    virtual ~TestContext() = default;
};

/**
 * Container for test cases and child test groups.
 *
 * Maps to Mocha's describe declaration. TestSuite specifies the describe description, test cases (it) and
 * lifecycle hooks (before, after, beforeEach, afterEach).
 */
class TestSuite : public Napi::ObjectWrap<TestSuite> {
 public:
    explicit TestSuite(const Napi::CallbackInfo& info);
    virtual ~TestSuite();

    static Napi::Object New(Napi::Env env, const std::string& description);

    /**
     * Create a new TestSuite and populate it with tests using a list TestBuilderFunctions.
     */
    static Napi::Object Build(Napi::Env env, const std::string& description,
        const std::initializer_list<TestBuilderFunction> testBuilders);

    /**
     * Create a new child TestSuite.
     */
    TestSuite* Describe(const std::string& description);

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
    // Test specific data attached this test suite. Optional.
    std::shared_ptr<TestContext> context;

 private:
    Napi::Value GetDescription(const Napi::CallbackInfo& info);
    Napi::Value GetTests(const Napi::CallbackInfo& info);
    Napi::Value GetChildren(const Napi::CallbackInfo& info);
    Napi::Value GetBefore(const Napi::CallbackInfo& info);
    Napi::Value GetAfter(const Napi::CallbackInfo& info);
    Napi::Value GetBeforeEach(const Napi::CallbackInfo& info);
    Napi::Value GetAfterEach(const Napi::CallbackInfo& info);
    Napi::Value FunctionOrUndefined(Napi::Env env, TestFunction func);
    
 private:
    std::string description;
    std::vector<TestSuite*> children;
};

/**
 * Denotes a test failure. This exception type will be converted to Napi::Error and reported as a test failure to
 * the javascript test runner.
 */
class AssertionError : public std::exception {
 public:
    AssertionError(const std::string& failure, const std::string& message) noexcept;
    virtual ~AssertionError() noexcept = default;

    const char* what() const noexcept override {
        return this->error.c_str();
    }

    const std::string& GetError() const noexcept {
        return this->error;
    }

 private:
    std::string error;
};

/**
 * Test assertion methods.
 *
 * To be compatible with the javascript test environment, assertion errors are thrown using napi exceptions.
 */
struct Assert {
    /**
     * Checks that two values are equal using the == operator.
     */
    template<typename S, typename T>
    static void Equal(const S& value, const T& expected, const std::string& message = "") {
        if (!(value == expected)) {
            std::stringstream ss;

            ss << "Expected " << value << " to be equal to " << expected;

            throw AssertionError(ss.str(), message);
        }
    }

    /**
     * Checks that the value is equal to nullptr.
     */
    template<typename T>
    static void IsNull(const T& value, const std::string& message = "") {
        if (!(value == nullptr)) {
            std::stringstream ss;

            ss << "Expected " << value << " to be nullptr";

            throw AssertionError(ss.str(), message);
        }
    }

    /**
     * Checks that the value is not equal to nullptr.
     */
    template<typename T>
    static void IsNotNull(const T& value, const std::string& message = "") {
        if (value == nullptr) {
            std::stringstream ss;

            ss << "Expected " << value << " to not be nullptr";

            throw AssertionError(ss.str(), message);
        }
    }

    /**
     * Checks that the value is true.
     */
    static void IsTrue(const bool value, const std::string& message = "") {
        if (!value) {
            throw AssertionError("Expected true", message);
        }
    }

    /**
     * Checks that the value is false.
     */
    static void IsFalse(const bool value, const std::string& message = "") {
        if (value) {
            throw AssertionError("Expected false", message);
        }
    }

    /**
     * Checks that calling the passed in lambda function will throw an std::exception.
     */
    static void Throws(std::function<void()> func, const std::string& message = "") {
        try {
            func();
        } catch (const std::exception&) {
            return;
        }

        throw AssertionError("Expected function to throw an std::exception.", message);
    }

    /**
     * Fail the test immediately by throwing an exception.
     */
    static void Fail(const std::string& message = "Fail") {
        throw AssertionError("", message);
    }
};

inline
AssertionError::AssertionError(const std::string& failure, const std::string& message) noexcept {
    std::stringstream ss;

    ss << "AssertionError";

    if (!failure.empty()) {
        ss << ": " << failure;
    }

    if (!message.empty()) {
        ss << ": " << message;
    }

    this->error = ss.str();
}

inline
TestSuite::TestSuite(const CallbackInfo& info) : ObjectWrap<TestSuite>(info) {
    if (info[0].IsString()) {
        this->description = info[0].As<String>();
    }
}

inline
TestSuite::~TestSuite() {
    for (auto& child : this->children) {
        child->Unref();
    }
}

inline
Object TestSuite::New(Napi::Env env, const std::string& description) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "TestSuite", {
            InstanceAccessor("description", &TestSuite::GetDescription, nullptr),
            InstanceAccessor("tests", &TestSuite::GetTests, nullptr),
            InstanceAccessor("children", &TestSuite::GetChildren, nullptr),
            InstanceAccessor("before", &TestSuite::GetBefore, nullptr),
            InstanceAccessor("after", &TestSuite::GetAfter, nullptr),
            InstanceAccessor("beforeEach", &TestSuite::GetBeforeEach, nullptr),
            InstanceAccessor("afterEach", &TestSuite::GetAfterEach, nullptr),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.New({ String::New(env, description) });
}

inline
Object TestSuite::Build(Napi::Env env, const std::string& description,
        const std::initializer_list<TestBuilderFunction> testBuilders) {
    auto TestSuite{ TestSuite::New(env, description) };
    auto parent{ TestSuite::Unwrap(TestSuite) };

    for (auto& testBuilder : testBuilders) {
        testBuilder(env, parent);
    }

    return TestSuite;
}

inline
Napi::Value TestSuite::GetDescription(const CallbackInfo& info) {
    return String::New(info.Env(), this->description);
}

inline
Napi::Value TestSuite::GetTests(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);
    auto result{ Array::New(env, this->tests.size()) };
    auto i{ 0u };

    for (auto& test : this->tests) {
        auto object{ Object::New(env) };
        auto jsSafeTestFunc = [f = test.func](const Napi::CallbackInfo& info) {
            try {
                f(info);
            } catch (const AssertionError& e) {
                throw Error::New(info.Env(), e.what());
            } catch (const Error&) {
                throw;
            } catch (const std::exception& e) {
                throw Error::New(info.Env(), std::string("Unknown std::exception: ") + e.what());
            } catch (...) {
                throw Error::New(info.Env(), "Unknown native exception!");
            }
        };

        object["description"] = String::New(env, test.description);
        object["func"] = Function::New(env, jsSafeTestFunc);

        result[i++] = object;
    }

    return scope.Escape(result);
}

inline
Napi::Value TestSuite::GetChildren(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);
    auto result{ Array::New(env, this->children.size()) };
    auto i{ 0u };

    for (auto& child : this->children) {
        result[i++] = child->Value();
    }

    return scope.Escape(result);
}

inline
Napi::Value TestSuite::GetBefore(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->before);
}

inline
Napi::Value TestSuite::GetAfter(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->after);
}

inline
Napi::Value TestSuite::GetBeforeEach(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->beforeEach);
}

inline
Napi::Value TestSuite::GetAfterEach(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->afterEach);
}

inline
TestSuite* TestSuite::Describe(const std::string& description) {
    auto child{ TestSuite::Unwrap(TestSuite::New(this->Env(), description)) };

    child->Ref();
    this->children.push_back(child);

    return child;
}

inline
Napi::Value TestSuite::FunctionOrUndefined(Napi::Env env, TestFunction func) {
    if (func) {
        return Function::New(env, func);
    }

    return env.Undefined();
}

} // namespace Napi
