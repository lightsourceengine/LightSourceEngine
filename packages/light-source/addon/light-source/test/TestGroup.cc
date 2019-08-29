/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include "TestGroup.h"

using Napi::Array;
using Napi::CallbackInfo;
using Napi::EscapableHandleScope;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Object;
using Napi::ObjectWrap;
using Napi::String;

namespace ls {

Napi::Value FunctionOrUndefined(Napi::Env env, TestFunction func);

TestGroup::TestGroup(const CallbackInfo& info) : ObjectWrap<TestGroup>(info) {
    if (info[0].IsString()) {
        this->description = info[0].As<String>();
    }
}

TestGroup::~TestGroup() {
    for (auto& child : this->children) {
        child->Unref();
    }
}

Object TestGroup::New(Napi::Env env, const std::string& description) {
    static FunctionReference constructor;

    if (constructor.IsEmpty()) {
        HandleScope scope(env);

        auto func = DefineClass(env, "TestGroup", {
            InstanceAccessor("description", &TestGroup::GetDescription, nullptr),
            InstanceAccessor("tests", &TestGroup::GetTests, nullptr),
            InstanceAccessor("children", &TestGroup::GetChildren, nullptr),
            InstanceAccessor("before", &TestGroup::GetBefore, nullptr),
            InstanceAccessor("after", &TestGroup::GetAfter, nullptr),
            InstanceAccessor("beforeEach", &TestGroup::GetBeforeEach, nullptr),
            InstanceAccessor("afterEach", &TestGroup::GetAfterEach, nullptr),
        });

        constructor.Reset(func, 1);
        constructor.SuppressDestruct();
    }

    return constructor.New({ String::New(env, description) });
}

Napi::Value TestGroup::GetDescription(const CallbackInfo& info) {
    return String::New(info.Env(), this->description);
}

Napi::Value TestGroup::GetTests(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);
    auto result{ Array::New(env, this->tests.size()) };
    auto i{ 0u };

    for (auto& test : this->tests) {
        auto object{ Object::New(env) };

        object["description"] = String::New(env, test.description);
        object["func"] = Function::New(env, test.func);

        result[i++] = object;
    }

    return scope.Escape(result);
}

Napi::Value TestGroup::GetChildren(const CallbackInfo& info) {
    auto env{ info.Env() };
    EscapableHandleScope scope(env);
    auto result{ Array::New(env, this->children.size()) };
    auto i{ 0u };

    for (auto& child : this->children) {
        result[i++] = child->Value();
    }

    return scope.Escape(result);
}

Napi::Value TestGroup::GetBefore(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->before);
}

Napi::Value TestGroup::GetAfter(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->after);
}

Napi::Value TestGroup::GetBeforeEach(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->beforeEach);
}

Napi::Value TestGroup::GetAfterEach(const CallbackInfo& info) {
    return FunctionOrUndefined(info.Env(), this->afterEach);
}

TestGroup* TestGroup::AddChild(const std::string& description) {
    auto child{ TestGroup::Unwrap(TestGroup::New(this->Env(), description)) };

    child->Ref();
    this->children.push_back(child);

    return child;
}

TestGroup* TestGroup::Describe(const std::string& description) {
    return this->AddChild(description);
}

Napi::Value FunctionOrUndefined(Napi::Env env, TestFunction func) {
    if (func) {
        return Function::New(env, func);
    } else {
        return env.Undefined();
    }
}

} // namespace ls
