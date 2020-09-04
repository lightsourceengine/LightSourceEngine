/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source
 * tree.
 */

#include <ls/bindings/JSStyle.h>

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
            LS_FOR_EACH_STYLE_PROPERTY(LS_PROP)
        });

        #undef LS_PROP
    }

    return jsStyleConstructor.Value();
}

} // namespace bindings
} // namespace ls
