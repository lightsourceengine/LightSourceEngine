/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/bindings/JSStyle.h>

#include <ls/Style.h>
#include <ls/bindings/JSStyleClass.h>
#include <cassert>

using Napi::CallbackInfo;
using Napi::Function;
using Napi::FunctionReference;
using Napi::HandleScope;

static FunctionReference jsStyleConstructor;

namespace ls {
namespace bindings {

void JSStyle::Constructor(const CallbackInfo& info) {
    this->CreateNative();
}

Function JSStyle::GetClass(Napi::Env env) {
    if (jsStyleConstructor.IsEmpty()) {
        HandleScope scope(env);

        #define LS_PROP(ENUM) InstanceAccessor(                                         \
            ToString(StyleProperty::ENUM),                                              \
            [](JSStyle* instance, const CallbackInfo& info) {                           \
                return instance->Get(StyleProperty::ENUM, info);                        \
            },                                                                          \
            [](JSStyle* instance, const CallbackInfo& info, const Napi::Value& value) { \
                instance->Set(info.Env(), StyleProperty::ENUM, value);                  \
            }),

        jsStyleConstructor = DefineClass(env, "Style", true, {
            InstanceMethod("reset", &JSStyle::Reset),
            InstanceMethod(Napi::SymbolFor(env, "bindStyleClass"), &JSStyle::BindStyleClass),
            LS_FOR_EACH_STYLE_PROPERTY(LS_PROP)
        });

        #undef LS_PROP
    }

    return jsStyleConstructor.Value();
}

void JSStyle::Reset(const Napi::CallbackInfo& info) {
    assert(this->native != nullptr);
    // TODO: send change events
    this->native->Reset();
}

Napi::Value JSStyle::BindStyleClass(const Napi::CallbackInfo& info) {
    assert(this->native != nullptr);
    auto env{ info.Env() };
    auto value{ info[0] };

    if (Napi::IsNullish(env, value)) {
        this->native->SetParent(nullptr);
        return env.Null();
    }

    auto jsStyleClass{ JSStyleClass::Cast(info[0]) };

    NAPI_EXPECT_NOT_NULL(env, jsStyleClass, "bindStyleClass requires a StyleClass instance or null");

    this->native->SetParent(jsStyleClass->GetNative());

    return info[0];
}

} // namespace bindings
} // namespace ls
