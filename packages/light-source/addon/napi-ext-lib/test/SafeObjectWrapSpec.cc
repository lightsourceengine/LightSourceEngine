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
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;
using Napi::Number;
using Napi::Object;
using Napi::ObjectReference;
using Napi::RunScript;
using Napi::SafeObjectWrap;
using Napi::TestInfo;
using Napi::TestSuite;
using Napi::Value;

namespace ls {

constexpr const auto STATIC_VALUE = 10;
constexpr const auto INSTANCE_VALUE = 20;

class A : public SafeObjectWrap<A> {
 public:
    static FunctionReference sClassA;

    A(const CallbackInfo& info) : SafeObjectWrap<A>(info) {
    }

    void Constructor(const CallbackInfo &info) override {
        this->constructorCalled = true;
    }

    static Function GetClass(const Napi::Env& env) {
        if (sClassA.IsEmpty()) {
            sClassA = A::DefineClass(
                env, "A", false,
                {
                    // static bindings
                    A::StaticMethod("staticFunc", &A::StaticFunc),
                    A::StaticMethod("staticFuncThrows", &A::StaticFuncThrows),
                    A::StaticMethod("staticFuncVoid", &A::StaticFuncVoid),
                    A::StaticMethod("staticFuncVoidThrows", &A::StaticFuncVoidThrows),
                    A::StaticAccessor("staticPropertyReadOnly", &A::GetStaticPropertyReadOnly),
                    A::StaticAccessor("staticPropertyReadOnlyThrows", &A::GetStaticPropertyReadOnlyThrows),
                    A::StaticAccessor("staticProperty", &A::GetStaticProperty, &A::SetStaticProperty),
                    A::StaticAccessor("staticPropertyThrows", &A::GetStaticPropertyThrows, &A::SetStaticPropertyThrows),
                    A::StaticValue("staticValue", Number::New(env, STATIC_VALUE)),
                    // instance bindings
                    A::InstanceMethod("func", &A::Func),
                    A::InstanceMethod("funcThrows", &A::FuncThrows),
                    A::InstanceMethod("funcVoid", &A::FuncVoid),
                    A::InstanceMethod("funcVoidThrows", &A::FuncVoidThrows),
                    A::InstanceAccessor("propertyReadOnly", &A::GetPropertyReadOnly),
                    A::InstanceAccessor("propertyReadOnlyThrows", &A::GetPropertyReadOnlyThrows),
                    A::InstanceAccessor("property", &A::GetProperty, &A::SetProperty),
                    A::InstanceAccessor("propertyThrows", &A::GetPropertyThrows, &A::SetPropertyThrows),
                    A::InstanceValue("value", Number::New(env, INSTANCE_VALUE)),
                });
        }

        return sClassA.Value();
    }

    static void StaticFuncVoid(const CallbackInfo &info) {
        staticFuncVoidCalled = true;
    }

    static void StaticFuncVoidThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    static Napi::Value StaticFunc(const CallbackInfo &info) {
        staticFuncCalled = true;
        return info.Env().Null();
    }

    static Napi::Value StaticFuncThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    static Napi::Value GetStaticPropertyReadOnly(const CallbackInfo &info) {
        getStaticPropertyReadOnlyCalled = true;
        return info.Env().Null();
    }

    static Napi::Value GetStaticPropertyReadOnlyThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    static Napi::Value GetStaticProperty(const CallbackInfo &info) {
        getStaticPropertyCalled = true;
        return info.Env().Null();
    }

    static Napi::Value GetStaticPropertyThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    static void SetStaticProperty(const CallbackInfo &info, const Napi::Value& value) {
        setStaticPropertyCalled = true;
    }

    static void SetStaticPropertyThrows(const CallbackInfo &info, const Napi::Value& value) {
        throw Error::New(info.Env());
    }

    void FuncVoid(const CallbackInfo &info) {
        this->funcVoidCalled = true;
    }

    void FuncVoidThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    Napi::Value Func(const CallbackInfo &info) {
        this->funcCalled = true;
        return info.Env().Null();
    }

    Napi::Value FuncThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    Napi::Value GetPropertyReadOnly(const CallbackInfo &info) {
        this->getPropertyReadOnlyCalled = true;
        return info.Env().Null();
    }

    Napi::Value GetPropertyReadOnlyThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    Napi::Value GetProperty(const CallbackInfo &info) {
        this->getPropertyCalled = true;
        return info.Env().Null();
    }

    Napi::Value GetPropertyThrows(const CallbackInfo &info) {
        throw Error::New(info.Env());
    }

    void SetProperty(const CallbackInfo &info, const Napi::Value& value) {
        this->setPropertyCalled = true;
    }

    void SetPropertyThrows(const CallbackInfo &info, const Napi::Value& value) {
        throw Error::New(info.Env());
    }

    static void ResetStatics() {
        staticFuncVoidCalled = false;
        staticFuncCalled = false;
        getStaticPropertyReadOnlyCalled = false;
        getStaticPropertyCalled = false;
        setStaticPropertyCalled = false;
    }

    static bool staticFuncVoidCalled;
    static bool staticFuncCalled;
    static bool getStaticPropertyReadOnlyCalled;
    static bool getStaticPropertyCalled;
    static bool setStaticPropertyCalled;

    bool constructorCalled{false};
    bool funcCalled{false};
    bool funcVoidCalled{false};
    bool getPropertyReadOnlyCalled{false};
    bool getPropertyCalled{false};
    bool setPropertyCalled{false};
};

bool A::staticFuncVoidCalled{false};
bool A::staticFuncCalled{false};
bool A::getStaticPropertyReadOnlyCalled{false};
bool A::getStaticPropertyCalled{false};
bool A::setStaticPropertyCalled{false};

class B : public SafeObjectWrap<B> {
 public:
    B(const CallbackInfo& info) : SafeObjectWrap<B>(info) {
    }

    static FunctionReference sClassB;

    static Function GetClass(const Napi::Env& env) {
        if (sClassB.IsEmpty()) {
            HandleScope scope(env);

            sClassB = B::DefineClass(env, "B", true, {});
        }

        return sClassB.Value();
    }
};

class C : public SafeObjectWrap<C> {
 public:
    C(const CallbackInfo& info) : SafeObjectWrap<C>(info) {
    }

    void Constructor(const CallbackInfo &info) override {
        throw Error::New(info.Env(), "from C");
    }

    static FunctionReference sClassC;

    static Function GetClass(const Napi::Env& env) {
        if (sClassC.IsEmpty()) {
            HandleScope scope(env);

            sClassC = C::DefineClass(env, "C", true, {});
        }

        return sClassC.Value();
    }
};

FunctionReference A::sClassA;
FunctionReference B::sClassB;
FunctionReference C::sClassC;

static Napi::Value GetGlobalInstanceA(const Napi::Env& env) {
    return env.Global().Get("SafeObjectWrapSpec").As<Object>().Get("a");
}

void SafeObjectWrapSpec(TestSuite* parent) {
    auto spec{ parent->Describe("SafeObjectWrap") };

    spec->before = [](const Napi::Env& env) {
        HandleScope scope(env);

        // Expose to js context so tests can create these classes via RunScript.
        auto context{ Object::New(env) };

        context.Set("A", A::GetClass(env));
        context.Set("B", B::GetClass(env));
        context.Set("C", C::GetClass(env));
        context.Set("a", A::GetClass(env).New({}));

        env.Global().Set("SafeObjectWrapSpec", context);
    };

    spec->after = [](const Napi::Env& env) {
        env.Global().Delete("SafeObjectWrapSpec");
    };

    spec->beforeEach = [](const Napi::Env& env) {
        A::ResetStatics();
    };

    spec->Describe("Constructor()")->tests = {
        {
            "should call overridden Constructor()",
            [](const TestInfo& info) {
                auto jsObject{ A::GetClass(info.Env()).New({}) };

                Assert::IsTrue(A::Cast(jsObject)->constructorCalled);
            }
        },
        {
            "should throw Error from Constructor()",
            [](const TestInfo& info) {
                Assert::Throws([&](){ C::GetClass(info.Env()).New({}); });
            }
        },
        {
            "should call overridden Constructor() from javascript invocation",
            [](const TestInfo& info) {
                auto instance{ RunScript(info.Env(), "new SafeObjectWrapSpec.A()") };

                Assert::IsTrue(A::Cast(instance)->constructorCalled);
            }
        }
    };

    spec->Describe("StaticMethod()")->tests = {
        {
            "should call staticFunc()",
            [](const TestInfo& info) {
                RunScript(info.Env(), "SafeObjectWrapSpec.A.staticFunc()");

                Assert::IsTrue(A::staticFuncCalled);
            }
        },
        {
            "should call staticFuncVoid()",
            [](const TestInfo& info) {
                RunScript(info.Env(), "SafeObjectWrapSpec.A.staticFuncVoid()");

                Assert::IsTrue(A::staticFuncVoidCalled);
            }
        },
        {
            "should throw Error from static function",
            [](const TestInfo& info) {
                Assert::Throws([env = info.Env()](){
                    RunScript(env, "SafeObjectWrapSpec.A.staticFuncThrows()");
                });
            }
        },
        {
            "should throw Error from static void function",
            [](const TestInfo& info) {
                Assert::Throws([env = info.Env()](){
                    RunScript(env, "SafeObjectWrapSpec.A.staticFuncVoidThrows()");
                });
            }
        }
    };

    spec->Describe("StaticValue()")->tests = {
        {
            "should set staticValue to 10",
            [](const TestInfo &info) {
                auto staticValue{ RunScript(info.Env(), "SafeObjectWrapSpec.A.staticValue") };

                Assert::IsTrue(staticValue.IsNumber());
                Assert::Equal(staticValue.As<Number>().Int32Value(), STATIC_VALUE);
            }
        },
    };

    spec->Describe("StaticAccessor()")->tests = {
        {
            "should call staticPropertyReadOnly",
            [](const TestInfo &info) {
                RunScript(info.Env(), "SafeObjectWrapSpec.A.staticPropertyReadOnly");

                Assert::IsTrue(A::getStaticPropertyReadOnlyCalled);
            }
        },
        {
            "should get and set staticProperty",
            [](const TestInfo &info) {
                RunScript(info.Env(), "SafeObjectWrapSpec.A.staticProperty");
                Assert::IsTrue(A::getStaticPropertyCalled);

                RunScript(info.Env(), "SafeObjectWrapSpec.A.staticProperty = null");
                Assert::IsTrue(A::setStaticPropertyCalled);
            }
        },
        {
            "should throw Error getting staticPropertyReadOnlyThrows",
            [](const TestInfo &info) {
                Assert::Throws([env = info.Env()]() {
                    RunScript(env, "SafeObjectWrapSpec.A.staticPropertyReadOnlyThrows");
                });
            }
        },
        {
            "should throw Error accessing staticPropertyThrows",
            [](const TestInfo &info) {
                Assert::Throws([env = info.Env()]() {
                    RunScript(env, "SafeObjectWrapSpec.A.staticPropertyThrows");
                });

                Assert::Throws([env = info.Env()]() {
                    RunScript(env, "SafeObjectWrapSpec.A.staticPropertyThrows = null");
                });
            }
        },
    };

    spec->Describe("InstanceMethod()")->tests = {
        {
            "should call func()",
            [](const TestInfo& info) {
                RunScript(info.Env(), "SafeObjectWrapSpec.a.func()");
                Assert::IsTrue(A::Cast(GetGlobalInstanceA(info.Env()))->funcCalled);
            }
        },
        {
            "should call funcVoid()",
            [](const TestInfo& info) {
                RunScript(info.Env(), "SafeObjectWrapSpec.a.funcVoid()");
                Assert::IsTrue(A::Cast(GetGlobalInstanceA(info.Env()))->funcVoidCalled);
            }
        },
        {
            "should throw Error from func()",
            [](const TestInfo& info) {
                Assert::Throws([env = info.Env()](){
                    RunScript(env, "SafeObjectWrapSpec.a.funcThrows()");
                });
            }
        },
        {
            "should throw Error from void funcVoid()",
            [](const TestInfo& info) {
                Assert::Throws([env = info.Env()](){
                    RunScript(env, "SafeObjectWrapSpec.a.funcVoidThrows()");
                });
            }
        }
    };

    spec->Describe("InstanceValue()")->tests = {
        {
            "should set value property to 20",
            [](const TestInfo &info) {
                auto staticValue{ RunScript(info.Env(), "SafeObjectWrapSpec.a.value") };

                Assert::IsTrue(staticValue.IsNumber());
                Assert::Equal(staticValue.As<Number>().Int32Value(), INSTANCE_VALUE);
            }
        },
    };

    spec->Describe("InstanceAccessor()")->tests = {
        {
            "should call propertyReadOnly",
            [](const TestInfo &info) {
                RunScript(info.Env(), "SafeObjectWrapSpec.a.propertyReadOnly");
                Assert::IsTrue(A::Cast(GetGlobalInstanceA(info.Env()))->getPropertyReadOnlyCalled);
            }
        },
        {
            "should get and set property",
            [](const TestInfo &info) {
                auto instance{ GetGlobalInstanceA(info.Env()) };

                RunScript(info.Env(), "SafeObjectWrapSpec.a.property");
                Assert::IsTrue(A::Cast(instance)->getPropertyCalled);

                RunScript(info.Env(), "SafeObjectWrapSpec.a.property = null");
                Assert::IsTrue(A::Cast(instance)->setPropertyCalled);
            }
        },
        {
            "should throw Error getting propertyReadOnlyThrows",
            [](const TestInfo &info) {
                Assert::Throws([env = info.Env()]() {
                    RunScript(env, "SafeObjectWrapSpec.a.propertyReadOnlyThrows");
                });
            }
        },
        {
            "should throw Error accessing propertyThrows",
            [](const TestInfo &info) {
                Assert::Throws([env = info.Env()]() {
                    RunScript(env, "SafeObjectWrapSpec.a.propertyThrows");
                });

                Assert::Throws([env = info.Env()]() {
                    RunScript(env, "SafeObjectWrapSpec.a.propertyThrows = null");
                });
            }
        },
    };

    spec->Describe("Cast()")->tests = {
        {
            "should get derived interface",
            [](const TestInfo& info) {
                auto jsObject{ A::GetClass(info.Env()).New({}) };

                Assert::IsNotNull(A::Cast(jsObject));
            }
        },
        {
            "should return null for non-derived interface",
            [](const TestInfo& info) {
                auto jsObject{ B::GetClass(info.Env()).New({}) };

                Assert::IsNull(A::Cast(jsObject));
            }
        },
        {
            "should return null for non SafeObjectWrap javascript objects",
            [](const TestInfo& info) {
                Assert::IsNull(A::Cast(Value()));
                Assert::IsNull(A::Cast(info.Env().Null()));
                Assert::IsNull(A::Cast(info.Env().Undefined()));
                Assert::IsNull(A::Cast(Object::New(info.Env())));
                Assert::IsNull(A::Cast(Array::New(info.Env())));
            }
        }
    };
}

} // namespace ls
