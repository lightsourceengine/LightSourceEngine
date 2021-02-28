/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <napi.h>
#include <string>
#include <cstring>
#include <vector>
#include <functional>
#include <initializer_list>
#include <sstream>
#include <memory>

namespace Napi {

class TestSuite;
class TestInfo;

using TestFunction = std::function<void(const TestInfo&)>;
using TestSuiteFunction = std::function<void(Env)>;
using TestBuilderFunction = std::function<void(TestSuite*)>;

constexpr auto SkipTest{true};

/**
 * Test case function and description.
 *
 * Maps to parameters of Mocha's test declaration: it(description, function).
 */
struct Test {
  std::string description;
  TestFunction func;
  bool skip{};
};

/**
 * Test case environment info.
 */
class TestInfo {
 public:
  TestInfo(napi_env env) : env(env) {}

  Napi::Env Env() const noexcept { return env; }

 private:
  napi_env env;
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
class TestSuite {
 public:
  explicit TestSuite(std::string description);

  /**
   * Create a new TestSuite and populate it with tests using a list TestBuilderFunctions.
   */
  static Napi::Object Build(Napi::Env env, const std::string& description,
                            const std::initializer_list<TestBuilderFunction>& testBuilders);

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
  TestSuiteFunction before;
  // Function executed after all tests are run. Optional.
  TestSuiteFunction after;
  // Function executed before each test case is run. Optional.
  TestSuiteFunction beforeEach;
  // Function executed after each test case is run. Optional.
  TestSuiteFunction afterEach;
  // Test specific data attached this test suite. Optional.
  std::shared_ptr<TestContext> context;

 private:
  Napi::Object ToObject(const Napi::Env& env);
  Napi::Value GetDescription(const Napi::Env& env);
  Napi::Value GetTests(const Napi::Env& env);
  Napi::Value GetChildren(const Napi::Env& env);
  Napi::Value GetBefore(const Napi::Env& env);
  Napi::Value GetAfter(const Napi::Env& env);
  Napi::Value GetBeforeEach(const Napi::Env& env);
  Napi::Value GetAfterEach(const Napi::Env& env);
  Napi::Value TestSuiteFunctionOrUndefined(const Napi::Env& env, const TestSuiteFunction& func);

 private:
  std::string description;
  std::vector<TestSuite> children;
};

/**
 * Denotes a test failure. This exception type will be converted to Napi::Error and reported as a test failure to
 * the javascript test runner.
 */
class AssertionError : public std::exception {
 public:
  AssertionError(const std::string& failure, const std::string& message) noexcept;
  AssertionError(const AssertionError& e) noexcept = default;
  ~AssertionError() noexcept override = default;

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

  static void CStringEqual(const char* value, const char* expected, const std::string& message = "") {
    if (value == nullptr && expected == nullptr) {
      return;
    } else if (value != nullptr && expected != nullptr && strcmp(value, expected) == 0) {
      return;
    }

    std::stringstream ss;

    ss << "Expected " << (value ? value : "null") << " to be equal to " << (expected ? expected : "null");

    throw AssertionError(ss.str(), message);
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
  static void Throws(const std::function<void()>& func, const std::string& message = "") {
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
TestSuite::TestSuite(std::string description) : description(std::move(description)) {
}

inline
Object TestSuite::Build(Napi::Env env, const std::string& description,
                        const std::initializer_list<TestBuilderFunction>& testBuilders) {
  TestSuite parent(description);

  for (auto& testBuilder : testBuilders) {
    testBuilder(&parent);
  }

  return parent.ToObject(env);
}

inline
Napi::Object TestSuite::ToObject(const Napi::Env& env) {
  auto obj{ Object::New(env) };

  obj.Set("description", this->GetDescription(env));
  obj.Set("before", this->GetBefore(env));
  obj.Set("after", this->GetAfter(env));
  obj.Set("beforeEach", this->GetBeforeEach(env));
  obj.Set("afterEach", this->GetAfterEach(env));
  obj.Set("tests", this->GetTests(env));
  obj.Set("children", this->GetChildren(env));

  return obj;
}

inline
Napi::Value TestSuite::GetDescription(const Napi::Env& env) {
  return String::New(env, this->description);
}

inline
Napi::Value TestSuite::GetTests(const Napi::Env& env) {
  auto result{ Array::New(env, this->tests.size()) };
  auto i{ 0u };

  for (auto& test : this->tests) {
    auto object{ Object::New(env) };

    auto jsSafeTestFunc = [f = test.func](const Napi::CallbackInfo& info) {
      auto env{ info.Env() };
      HandleScope scope(env);
      const TestInfo testInfo(env);

      try {
        f(testInfo);
      } catch (const AssertionError& e) {
        throw Error::New(env, e.what());
      } catch (const Error&) {
        throw;
      } catch (const std::exception& e) {
        throw Error::New(env, std::string("Unknown std::exception: ") + e.what());
      } catch (...) {
        throw Error::New(env, "Unknown native exception!");
      }
    };

    object["description"] = String::New(env, test.description);
    object["func"] = Function::New(env, jsSafeTestFunc);
    object["skip"] = Boolean::New(env, test.skip);

    result[i++] = object;
  }

  return result;
}

inline
Napi::Value TestSuite::GetChildren(const Napi::Env& env) {
  auto result{ Array::New(env, this->children.size()) };
  auto i{ 0u };

  for (auto& child : this->children) {
    result[i++] = child.ToObject(env);
  }

  return result;
}

inline
Napi::Value TestSuite::GetBefore(const Napi::Env& env) {
  return TestSuiteFunctionOrUndefined(env, this->before);
}

inline
Napi::Value TestSuite::GetAfter(const Napi::Env& env) {
  return TestSuiteFunctionOrUndefined(env, this->after);
}

inline
Napi::Value TestSuite::GetBeforeEach(const Napi::Env& env) {
  return TestSuiteFunctionOrUndefined(env, this->beforeEach);
}

inline
Napi::Value TestSuite::GetAfterEach(const Napi::Env& env) {
  return TestSuiteFunctionOrUndefined(env, this->afterEach);
}

inline
TestSuite* TestSuite::Describe(const std::string& description) {
  this->children.emplace_back(description);

  return &this->children.back();
}

inline
Napi::Value TestSuite::TestSuiteFunctionOrUndefined(const Napi::Env& env, const TestSuiteFunction& func) {
  if (func) {
    return Function::New(env, [func](const Napi::CallbackInfo& info) { func(info.Env()); });
  }

  return env.Undefined();
}

} // namespace Napi
