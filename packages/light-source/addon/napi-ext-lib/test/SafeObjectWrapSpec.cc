/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#include <napi-unit.h>
#include <napi-ext.h>

using Napi::Array;
using Napi::Assert;
using Napi::CallbackInfo;
using Napi::Error;
using Napi::Object;
using Napi::QueryInterface;
using Napi::SafeObjectWrap;
using Napi::TestInfo;
using Napi::TestSuite;
using Napi::Value;

namespace ls {

class A : public SafeObjectWrap<A> {
 public:
    A(const CallbackInfo& info) : SafeObjectWrap<A>(info) {
    }

    void Constructor(const CallbackInfo &info) override {
        this->constructorCalled = true;
    }

    bool constructorCalled{false};

    friend SafeObjectWrap<A>;
};

class B : public SafeObjectWrap<B> {
 public:
    B(const CallbackInfo& info) : SafeObjectWrap<B>(info) {
    }

    friend SafeObjectWrap<B>;
};

class C : public SafeObjectWrap<C> {
 public:
    C(const CallbackInfo& info) : SafeObjectWrap<C>(info) {
    }

    void Constructor(const CallbackInfo &info) override {
        throw Error::New(info.Env(), "from C");
    }

    friend SafeObjectWrap<C>;
};

void SafeObjectWrapSpec(TestSuite* parent) {
    auto spec{ parent->Describe("SafeObjectWrap") };

    spec->Describe("Constructor()")->tests = {
        {
            "should call overridden Constructor()",
            [](const TestInfo& info) {
                auto ref{ A::DefineClass(info.Env(), "A", {}) };
                auto jsObject{ ref.New({}) };

                Assert::IsTrue(QueryInterface<A>(jsObject)->constructorCalled);
            }
        },
        {
            "should throw Error from Constructor()",
            [](const TestInfo& info) {
                auto ref{ C::DefineClass(info.Env(), "C", {}) };

                Assert::Throws([&](){ ref.New({}); });
            }
        }
    };

    spec->Describe("QueryInterface()")->tests = {
        {
            "should get derived interface",
            [](const TestInfo& info) {
                auto ref{ A::DefineClass(info.Env(), "A", {}) };
                auto jsObject{ ref.New({}) };

                Assert::IsNotNull(QueryInterface<A>(jsObject));
            }
        },
        {
            "should return null for non-derived interface",
            [](const TestInfo& info) {
                auto ref{ B::DefineClass(info.Env(), "B", {}) };
                auto jsObject{ ref.New({}) };

                Assert::IsNull(QueryInterface<A>(jsObject));
            }
        },
        {
            "should return null for non SafeObjectWrap javascript objects",
            [](const TestInfo& info) {
                Assert::IsNull(QueryInterface<A>(Value()));
                Assert::IsNull(QueryInterface<A>(info.Env().Null()));
                Assert::IsNull(QueryInterface<A>(info.Env().Undefined()));
                Assert::IsNull(QueryInterface<A>(Object::New(info.Env())));
                Assert::IsNull(QueryInterface<A>(Array::New(info.Env())));
            }
        }
    };
}

} // namespace ls
